#!/usr/bin/env bash
# Run from the folder containing the C sources, in Ubuntu Linux.
# For screenshots, paste one numbered section at a time into the same terminal.
set -eu

# 1. Producer and consumer.
gcc -Wall -pthread -o prodcons prodcons.c
./prodcons

# 2. Locking before waiting for an empty slot can deadlock.
sed '/sem_wait(&empty);/{N;s/sem_wait(&empty);\n        pthread_mutex_lock(&lock);/pthread_mutex_lock(\&lock);\n        sem_wait(\&empty);/;}' prodcons.c > prodcons_bad.c
diff -u prodcons.c prodcons_bad.c || true
gcc -Wall -pthread -o prodcons_bad prodcons_bad.c
status=0
timeout 5 stdbuf -oL ./prodcons_bad || status=$?
echo "exit code: $status"
./prodcons

# 3. Condition variable: while, then the experimental if, then while again.
gcc -Wall -pthread -o condvar condvar.c
./condvar
sed 's/while (!ready)/if (!ready)/' condvar.c > condvar_if.c
diff -u condvar.c condvar_if.c || true
gcc -Wall -pthread -o condvar_if condvar_if.c
./condvar_if
./condvar

# 4. Readers and writers. Repeat to observe scheduling variation.
gcc -Wall -pthread -o readers readers.c
./readers
./readers
./readers

# 5. Opposite lock order, with the original sleeps.
gcc -Wall -pthread -o deadlock_original deadlock_original.c
status=0
timeout 5 stdbuf -oL ./deadlock_original || status=$?
echo "exit code: $status"

# 6. Remove the sleeps from the ORIGINAL version and run ten times.
sed '/sleep(1);/d' deadlock_original.c > deadlock_nosleep.c
diff -u deadlock_original.c deadlock_nosleep.c || true
gcc -Wall -pthread -o deadlock_nosleep deadlock_nosleep.c
for i in {1..10}; do
    echo "Run $i"
    status=0
    timeout 2 stdbuf -oL ./deadlock_nosleep || status=$?
    echo "exit code: $status"
done

# 7. Final submission version: both threads acquire A before B.
diff -u deadlock_original.c deadlock.c || true
gcc -Wall -pthread -o deadlock deadlock.c
status=0
timeout 5 stdbuf -oL ./deadlock || status=$?
echo "exit code: $status"
