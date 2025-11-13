#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# ScopeTimer Utilities - Bash helper scripts
# Copyright (C) 2025 Steve Clarke <stephenlclarke@mac.com> https://xyzzy.tools
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
