#!/usr/bin/env bash
# Run from the directory containing the C files.
# For screenshots, paste one section at a time into the same Bash terminal.
set -eu

# Part 1: the unprotected counter.
gcc -Wall -pthread -o race race.c
./race
./race
./race

# Part 2: both locks, three runs each.
gcc -Wall -pthread -o mutex mutex.c
gcc -Wall -pthread -o sem sem.c
./mutex
./mutex
./mutex
./sem
./sem
./sem

# Compare the timings recorded by Bash's time keyword.
time ./race
time ./mutex
time ./sem

# Part 2 Observe: two permits no longer give mutual exclusion.
sed 's/sem_init(&s, 0, 1);/sem_init(\&s, 0, 2);/' sem.c > sem_two.c
diff -u sem.c sem_two.c || true
gcc -Wall -pthread -o sem_two sem_two.c
./sem_two
./sem_two
./sem_two

# Part 3: signal at zero, followed by the deliberately changed start value.
gcc -Wall -pthread -o order order.c
./order
sed 's/sem_init(&ready, 0, 0);/sem_init(\&ready, 0, 1);/' order.c > order_one.c
diff -u order.c order_one.c || true
gcc -Wall -pthread -o order_one order_one.c
./order_one
# The original still starts at zero.
./order

# Part 4: three parking places, then one place.
gcc -Wall -pthread -o parking parking.c
./parking
sed 's/#define PLACES 3/#define PLACES 1/' parking.c > parking_one.c
diff -u parking.c parking_one.c || true
gcc -Wall -pthread -o parking_one parking_one.c
./parking_one

# Optional: atomic increment and its timing.
gcc -Wall -pthread -o atomic atomic.c
time ./atomic
