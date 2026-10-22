#!/bin/bash
# Usage: ./report.sh access.log

log=$1
if [ ! -f "$log" ]; then
  echo "no such file: $log" >&2
  exit 1
fi

total=$(wc -l < "$log")
fails=$(grep -c ' FAIL ' "$log")

echo "$total requests, $fails failed"
for user in user{0..6}; do
  failures=$(grep -c " $user FAIL " "$log")
  echo "$user: $failures failures"
done
