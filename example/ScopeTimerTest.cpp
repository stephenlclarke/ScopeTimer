/*
 * ScopeTimer - lightweight C++17 scope timing utility
 * Copyright (C) 2025 Steve Clarke <stephenlclarke@mac.com>
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
#include "ScopeTimer.hpp"
#include <unistd.h>

int main() {
    SCOPE_TIMER("Steve's Test");

    usleep(2500);

    return 0;
}
