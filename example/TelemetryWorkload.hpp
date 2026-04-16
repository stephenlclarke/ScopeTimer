/*
 * ScopeTimer - lightweight C++17 scope timing utility
 * Copyright (C) 2025 Steve Clarke <stephenlclarke@mac.com> https://xyzzy.tools
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * In accordance with section 13 of the AGPL, if you modify this program,
 * your modified version must prominently offer all users interacting with it
 * remotely through a computer network an opportunity to receive the source
 * code of your version.
 */

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace xyzzy::scopetimer::example_workload {

struct TelemetryEvent {
    std::uint64_t requestId{};
    std::uint64_t accountId{};
    std::uint32_t routeKey{};
    std::uint32_t bytes{};
    std::uint16_t statusCode{};
    std::uint16_t flags{};
    std::array<std::uint8_t, 24> payload{};
};

struct TelemetryTotals {
    std::array<std::uint64_t, 16> shardLoad{};
    std::array<std::uint64_t, 32> routeBytes{};
    std::array<std::uint64_t, 8> statusBuckets{};
    std::uint64_t checksum{0};
    std::uint64_t retries{0};
};

inline std::uint64_t mix64(std::uint64_t value) noexcept {
    value += 0x9e3779b97f4a7c15ULL;
    value = (value ^ (value >> 30)) * 0xbf58476d1ce4e5b9ULL;
    value = (value ^ (value >> 27)) * 0x94d049bb133111ebULL;
    return value ^ (value >> 31);
}

inline std::uint16_t statusCodeForState(std::uint64_t state) noexcept {
    if ((state & 0x1fU) == 0) {
        return 503U;
    }
    if ((state & 0x3U) == 0) {
        return 429U;
    }
    return 200U;
}

inline std::size_t statusBucketForCode(std::uint16_t statusCode) noexcept {
    if (statusCode >= 500U) {
        return 5U;
    }
    if (statusCode >= 400U) {
        return 4U;
    }
    return 2U;
}

inline std::vector<TelemetryEvent> makeTelemetryBatch(std::size_t count) {
    std::vector<TelemetryEvent> batch;
    batch.reserve(count);

    std::uint64_t state = 0x123456789abcdef0ULL;
    for (std::size_t i = 0; i < count; ++i) {
        state = mix64(state + i + 0x9e3779b97f4a7c15ULL);

        TelemetryEvent event{};
        event.requestId = state;
        event.accountId = mix64(state ^ 0xa5a5a5a5a5a5a5a5ULL);
        event.routeKey = static_cast<std::uint32_t>((state >> 16) & 31U);
        event.bytes = 128U + static_cast<std::uint32_t>(state & 511U);
        event.statusCode = statusCodeForState(state);
        event.flags = static_cast<std::uint16_t>((state >> 20) & 0x3fU);

        for (auto& byte : event.payload) {
            state = mix64(state + byte + i);
            byte = static_cast<std::uint8_t>(state & 0xffU);
        }

        batch.push_back(event);
    }

    return batch;
}

inline std::uint64_t processTelemetryBatchBody(const std::vector<TelemetryEvent>& batch, TelemetryTotals& totals) {
    std::uint64_t checksum = totals.checksum;
    for (const auto& event : batch) {
        const auto shard = static_cast<std::size_t>(event.requestId & (totals.shardLoad.size() - 1U));
        const auto route = static_cast<std::size_t>(event.routeKey % totals.routeBytes.size());
        const auto statusBucket = static_cast<std::size_t>((event.statusCode / 100U) % totals.statusBuckets.size());

        totals.shardLoad[shard] += event.bytes + (event.flags & 0x0fU);
        totals.routeBytes[route] += event.bytes;
        totals.statusBuckets[statusBucket] += 1U;

        std::uint64_t rolling = mix64(event.requestId ^ (event.accountId << 1U) ^ event.flags);
        for (std::uint8_t byte : event.payload) {
            rolling ^= static_cast<std::uint64_t>(byte) + totals.routeBytes[route] + 0x9e3779b97f4a7c15ULL;
            rolling = mix64(rolling + event.bytes + statusBucket);
        }

        checksum ^= rolling + totals.shardLoad[shard];
        if ((event.flags & 0x01U) && event.statusCode >= 500U) {
            ++totals.retries;
            checksum += totals.routeBytes[route] ^ event.accountId;
        }
        if (event.flags & 0x08U) {
            checksum ^= totals.statusBuckets[statusBucket] * 2654435761ULL;
        }
    }

    totals.checksum = checksum;
    return checksum;
}

inline void ingestTelemetryRecordBody(const TelemetryEvent& event, TelemetryTotals& totals, std::uint64_t salt) {
    const auto shard = static_cast<std::size_t>(event.requestId & (totals.shardLoad.size() - 1U));
    const auto route = static_cast<std::size_t>(event.routeKey & (totals.routeBytes.size() - 1U));
    const auto statusBucket = statusBucketForCode(event.statusCode);

    totals.shardLoad[shard] += event.bytes + (event.flags & 0x0fU);
    totals.routeBytes[route] += event.bytes;
    totals.statusBuckets[statusBucket] += 1U;

    std::uint64_t rolling = mix64(event.requestId ^ salt ^ event.accountId);
    for (std::uint8_t byte : event.payload) {
        rolling ^= static_cast<std::uint64_t>(byte) + totals.routeBytes[route];
        rolling = mix64(rolling + event.bytes + salt);
    }

    totals.checksum ^= rolling + totals.shardLoad[shard];
    if ((event.flags & 0x01U) && event.statusCode >= 500U) {
        ++totals.retries;
        totals.checksum += totals.routeBytes[route] ^ event.accountId;
    }
}

} // namespace xyzzy::scopetimer::example_workload
