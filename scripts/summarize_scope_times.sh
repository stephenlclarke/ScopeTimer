#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# ScopeTimer Utilities - Bash helper scripts
# Copyright (C) 2025 Steve Clarke <stephenlclarke@mac.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#
# In accordance with section 13 of the AGPL, if you modify this program,
# your modified version must prominently offer all users interacting with it
# remotely through a computer network an opportunity to receive the source
# code of your version.
# -----------------------------------------------------------------------------
#
# Group by the prefix (everything before "| elapsed=") and show:
#   1) Per-key two-line block:
#        <key>
#        <v1> <v2> <v3> ...   (all elapsed values in order seen)
#   2) Per-key summary: count, min, avg, max, trend (↑ = increasing, ↓ = decreasing, - = stable)
#
# Usage:
#   ./summarize_scope_times.sh [ScopeTimer.log.cleaned]
#   cat ScopeTimer.log.cleaned | ./summarize_scope_times.sh

set -euo pipefail

input="${1:-/dev/stdin}"

awk '
function to_us(val, unit,   x) {
  x = val + 0
  if (unit == "us")      return x
  else if (unit == "ms") return x * 1000
  else if (unit == "s")  return x * 1000000
  else return x
}
function fmt_us(us,   x) {
  x = us + 0
  # Treat inputs < 1us as nanoseconds and convert to ms
  if (x < 1) return sprintf("%.3fms", x/1000.0)
  if (x >= 1000000) return sprintf("%.3fs", x/1000000.0)
  if (x >= 1000)    return sprintf("%.3fms", x/1000.0)
  return sprintf("%.0fus", x)
}
function print_block(k, s, e,   i, col) {
  col = 0
  for (i = s; i <= e; i++) {
    if (col == 0) {
      printf("%s", fmt_us(series[k, i]))
      col = 1
    } else {
      printf(" %s", fmt_us(series[k, i]))
      col++
    }
    if (col == 10 || i == e) {
      print ""
      col = 0
    }
  }
}

# Collect
{
  # Find split point at: optional spaces, literal pipe, optional spaces, then "elapsed="
  if (match($0, /[[:space:]]*\|[[:space:]]*elapsed=/)) {
    key  = substr($0, 1, RSTART - 1)
    rest = substr($0, RSTART + RLENGTH)  # e.g. "8us", "5.194ms", "15.059s"

    # Determine unit (BSD awk-safe)
    unit = ""
    if (rest ~ /us$/)      unit = "us"
    else if (rest ~ /ms$/) unit = "ms"
    else if (rest ~ /s$/)  unit = "s"

    if (unit != "") {
      num = rest
      sub(/(us|ms|s)$/, "", num)   # strip unit
      if (num ~ /^[0-9]+(\.[0-9]+)?$/) {
        us = to_us(num, unit)

        if (!(key in kseen)) {
          kseen[key] = 1
          korder[++kcnt] = key     # remember first-seen order
          c[key] = 0
          sum[key] = 0
          min[key] = us
          max[key] = us
        }

        seq = ++c[key]
        series[key, seq] = us
        sum[key] += us
        if (us < min[key]) min[key] = us
        if (us > max[key]) max[key] = us
      }
    }
  }
}

# Output
END {
  print "===== Summary (count / min / avg / max) ====="
  for (ki = 1; ki <= kcnt; ki++) {
    k = korder[ki]
    n = c[k]
    avg = (n ? sum[k] / n : 0)

    # Compute trend based on first 10% vs last 10% averages (ceil), with noise guard
    trend = "-"
    if (n >= 5) {
      nseg = int((n + 9) / 10)   # ceil(n*0.1)
      if (nseg < 1) nseg = 1
      fsum = 0; lsum = 0
      for (i = 1; i <= nseg; i++) fsum += series[k, i]
      for (i = n - nseg + 1; i <= n; i++) lsum += series[k, i]
      favg = fsum / nseg
      lavg = lsum / nseg
      delta = lavg - favg
      rel = (favg > 0 ? delta / favg : 0)
      # Consider trend only if both absolute and relative change pass thresholds
      if (delta > 5 && rel > 0.05)        trend = "↑"
      else if (delta < -5 && -rel > 0.05) trend = "↓"
      else                                trend = "-"
    }

    printf("%s\n  count=%d  min=%s  avg=%s  max=%s  %s\n\n",
           k, n, fmt_us(min[k]), fmt_us(avg), fmt_us(max[k]), trend)
  }
}
' "$input"
