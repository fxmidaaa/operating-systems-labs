#!/usr/bin/env bash
# Repeat the experiments; the original measured results are in report.md.
set -eu

source_dir=$(cd -- "$(dirname -- "$0")" && pwd)
run_dir="$HOME/os-lab9/run-$(date -u +%Y%m%d-%H%M%S)-$$"
mkdir -p "$run_dir"
cp "$source_dir/syscalls.c" "$run_dir/"
cd "$run_dir"
export LC_ALL=C

date -u -Is
whoami
hostname
pwd
uname -r
gcc --version | head -n 1
strace --version | head -n 1

strace -f -e trace=openat,read,write,close -o ls.trace ls -l
cat ls.trace
grep -Ec '(openat|read|write|close)\(' ls.trace
grep 'write(1,' ls.trace
strace -f -c -o ls-summary.txt ls -l
cat ls-summary.txt

gcc -Wall -Wextra -o syscalls syscalls.c
./syscalls
printf 'Program exit status: %s\n' "$?"
cat note.txt
wc -c note.txt
strace -e trace=openat,write,read,close -o syscalls.trace ./syscalls
cat syscalls.trace
