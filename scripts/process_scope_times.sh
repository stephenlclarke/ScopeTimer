#!/usr/bin/env bash
#
# Strip "| start=... | end=..." timestamp segments from ScopeTimer lines.
# Matches timestamps in the format: YYYY-MM-DD HH:MM:SS[.fractions] where the fractional
# seconds are optional and may contain 1-9 digits.
#
# Usage:
#   ./process_scope_times.sh [ScopeTimer.log] > ScopeTimer.log.cleaned
#
# Notes:
#   - If no file is given, reads from stdin.
#   - Prints to stdout. Redirect to a new file if needed.
#   - Does not modify the input file in place.

set -euo pipefail

# Regex pieces:
#   - Date:      [0-9]{4}-[0-9]{2}-[0-9]{2}
#   - Time:      [0-9]{2}:[0-9]{2}:[0-9]{2}
#   - Fraction:  (\.[0-9]{1,9})?   # optional, 1 to 9 digits (e.g., .7, .73, .735, .735123456)
#
# Full match we remove:
#   | start=YYYY-MM-DD HH:MM:SS[.fraction] | end=YYYY-MM-DD HH:MM:SS[.fraction]
#
# We use extended regex (-E). The pipes are escaped to match literal '|'.

input="${1:-/dev/stdin}"

script='s# TID=[0-9]+ \| ##g; s# \| start=[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}(\.[0-9]{1,9})? \| end=[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}(\.[0-9]{1,9})? ##g'

sed -E "$script" "$input"
