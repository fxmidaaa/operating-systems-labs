# Lab 11 The Race and the Semaphore

Cimendur Rustem, FAF-243. Experiments performed on 15 November 2026.

## The Race and the Semaphore

### Purpose of the work

I used two Linux threads to demonstrate lost updates on a shared counter, then protected the increment with a mutex and a semaphore. I also used semaphores to order a message exchange and limit access to parking places. Both locks produced the correct counter in every recorded run, and the parking occupancy stayed within the selected capacity.

### Environment and preparation

I worked in Ubuntu Linux inside VirtualBox as vboxuser on host rustem on 15 November 2026. The recorded time was 18:06:10 UTC, the kernel was 7.0.0-38-generic, and GCC was 15.2.0 from the Ubuntu 15.2.0-16ubuntu1 package. The working directory was /home/vboxuser/os-lab11/lab11. I compiled with -Wall -pthread and without optimization flags.

![Figure 1.1. Environment compilation and three runs of the unprotected counter](screenshots/image(20261115-180758).png)

### Part 1 Observe answers

1. counter++ is a read-modify-write operation: read the old value, add one to the local value, and write the new value back. It is not an atomic operation on this shared long.

2. If both threads read 10 before either writes, each computes 11 and stores 11. Two attempted increments have increased the counter only once. The three recorded totals were 1231830, 1022067 and 1040272 instead of 2000000.

3. Scheduling and execution overlap change between runs, so the threads can interleave differently. The program contains a C data race and has undefined behavior; lost updates explain the observed results, but no particular total is guaranteed.

## Part 2 Protecting the shared counter

Each of the two threads performs one million increments. In mutex.c, pthread_mutex_lock and pthread_mutex_unlock surround counter++. In sem.c, sem_wait and sem_post surround the same statement, with the semaphore initialized to one. This permits only one thread to enter the critical section at a time.

```c
pthread_mutex_lock(&lock);
counter++;
pthread_mutex_unlock(&lock);

sem_wait(&s);
counter++;
sem_post(&s);
```

![Figure 2.1. Three mutex runs and three semaphore runs all produce 2000000](screenshots/image(20261115-180758).png)

### Observe 1 Results with each lock

Yes. All three mutex runs and all three semaphore runs printed counter = 2000000 (expected 2000000). Their additional timed runs also produced 2000000. Joining both threads ensures that main prints only after their work has finished.

### Observe 3 Why locking takes longer

Every increment now requires acquiring and releasing a synchronization primitive. Contention can make a thread wait, and access to the shared counter is serialized. These costs make the protected runs slower in this measurement. The unprotected run is faster here because it omits synchronization, but it does not perform the work correctly.

## Part 2 Allowing two threads through

I created sem_two.c as a separate experimental variant, changing only the initial semaphore value from one to two. The original sem.c retained the correct lock configuration.

```c
sem_init(&s, 0, 2);
```

![Figure 2.2. The diff changes one to two and the counter again loses increments](screenshots/image(20261115-180815).png)

### Observe 2 Why a value of two breaks the lock

The three results were 1793321, 1733887 and 1788827. A starting value of two gives the semaphore two permits. Both threads can pass sem_wait before either calls sem_post, so both may execute counter++ at the same time. There is no longer mutual exclusion, and the shared non-atomic counter has a data race.

The semaphore operations are synchronized, but that alone does not make the statements between them safe. The initial permit count determines how many threads may enter. With one permit, the increment is protected; with two permits and two incrementing threads, it is not.

As with the original race, a correct-looking total could occur by chance. The important failure is the missing exclusive access, demonstrated by all three recorded totals falling below 2000000.

## Part 3 A semaphore as a signal

The writer waits for one second, copies hello into the message buffer, prints its message, and posts ready. The reader waits on ready before reading the buffer. Starting ready at zero prevents the reader from continuing until the writer signals completion.

![Figure 3.1. Zero waits for hello one allows an empty read and zero restores the order](screenshots/image(20261115-180925).png)

### Observe 1 Starting ready at one

In order_one.c, B printed I read "" before A printed message written. The initial permit let B pass sem_wait immediately, during the writer pause. The global message array is initially zero-filled, so it appeared as an empty string in this run. This configuration no longer guarantees that the write happens before the read.

### Observe 2 Meaning of a semaphore starting at zero

A semaphore initialized to zero means that no permit is available, so a waiter must wait until another thread posts a signal.

The final ./order run used the unchanged original source with ready initialized to zero. It again printed B waiting, A message written, and B reading "hello", in that order.

## Part 4 A semaphore as a resource counter

parking.c creates six car threads and initializes places to PLACES, which is three. A car must acquire a permit before it enters. It returns that permit only after leaving. A mutex separately protects the inside counter and its associated printf calls.

![Figure 4.1. Six cars complete while the recorded occupancy never exceeds three](screenshots/image(20261115-181525).png)

### Observe 1 Maximum occupancy and the limiting line

The largest printed value of inside was 3. Cars 1, 2 and 3 entered first. Later entries reused places released by departing cars. All six cars left, and the last recorded occupancy was 0.

```c
sem_init(&places, 0, PLACES);  /* PLACES is 3 */

sem_wait(&places);            /* before entering */
/* enter, park, and leave */
sem_post(&places);            /* after leaving */
```

sem_wait(&places), together with the initial count of three and one matching sem_post per departure, enforces the limit. When no permit remains, another car cannot pass the wait until a place is released.

### Why a mutex is also present

The semaphore allows up to three cars into the parking area, so it cannot by itself give exclusive access to inside. The mutex serializes updates to that shared counter and keeps each update together with its printed occupancy. The parking capacity is enforced by the semaphore, while the mutex protects the accounting.

## Part 4 Restricting parking to one car

I created parking_one.c by changing only #define PLACES 3 to #define PLACES 1. The six car threads and the rest of the program stayed the same.

![Figure 4.2. With one place every entry is followed by a departure before another entry](screenshots/image(20261115-181548).png)

### Observe 2 What the semaphore becomes

With one permit and balanced wait/post operations, the semaphore behaves as a binary semaphore and provides mutual exclusion for the entire parked interval. Only one car can occupy the parking area at a time. The printed occupancy alternated between 1 on entry and 0 on departure.

The recorded entry order was 2, 3, 4, 1, 5, 6. A car identifier does not determine its scheduling order. All six cars completed, and inside returned to zero. A semaphore used this way has a locking role, although a mutex additionally has an ownership rule.

## Timing comparison and the optional atomic counter

The table records Bash time output from the VM. Real is elapsed wall time; user and sys are CPU time in user and kernel code, accumulated across threads. These measurements describe these runs and do not establish a fixed performance order on other machines.

| Program | Counter | Real s | User s | Sys s |
| --- | --- | --- | --- | --- |
| race | 1325455 | 0.009 | 0.008 | 0.003 |
| mutex | 2000000 | 0.076 | 0.075 | 0.059 |
| sem | 2000000 | 0.123 | 0.072 | 0.152 |
| atomic | 2000000 | 0.018 | 0.030 | 0.001 |

![Figure 2.3. Recorded timing of the unprotected mutex and semaphore runs](screenshots/image(20261115-180758).png)

### Optional atomic increment

atomic.c declares an atomic_long and increments it with atomic_fetch_add. Each increment is an indivisible atomic read-modify-write operation. This run produced 2000000 in 0.018 seconds without an explicit mutex or semaphore around the increment.

![Figure 5.1. The optional atomic counter produces 2000000](screenshots/image(20261115-181559).png)

### Conclusion

The experiments showed why a shared read-modify-write needs synchronization. A mutex and a semaphore initialized to one protected the counter; zero permits established a readiness signal, and three permits limited resource use. Changing those counts demonstrated how the same semaphore can provide exclusion, ordering or capacity control. The optional atomic operation protected a single counter directly.

## Materials

The original assignment is in assignment.pdf. The C files and commands.sh reproduce the experiments. The original six screenshots are in screenshots/. The formatted report is ../lab11_report.docx.
