# Lab 12 Producer Consumer and the Other Tools

Cimendur Rustem, FAF-243.

These programs target Ubuntu Linux, as in Lab 11. Compile with GCC and
`-Wall -pthread`. They run in the Linux terminal, outside xv6.

| File | Experiment |
| --- | --- |
| prodcons.c | Correct producer/consumer with a four-slot circular buffer. |
| condvar.c | Readiness predicate, mutex and condition variable, using while. |
| readers.c | Three readers and two writers using two semaphores. |
| deadlock.c | Fixed submission version; both threads lock A before B. |
| deadlock_original.c | Deliberately broken version with opposite lock order. |
| prodcons_bad.c | Generated experiment: lock before waiting for an empty slot. |
| condvar_if.c | Generated experiment: replace while with if. |
| deadlock_nosleep.c | Generated experiment: remove sleeps from the broken version. |
| commands.sh | All required experiments, including the ten repeated runs. |
| assignment.pdf | Original seven-page laboratory handout. |
| screenshots/ | Eight original screenshots of the Ubuntu experiments. |
| diagrams/deadlock_cycle.png | Circular wait between the two threads and two mutexes. |

## Record the work

Run from this folder. Start a terminal transcript before the experiments:

```bash
export LC_ALL=C
script -q -f session.log
date -u -Is
whoami
hostname
pwd
uname -r
gcc --version | head -n 1
```

Paste each numbered section from commands.sh separately and take screenshots
that include the commands and output. Alternatively, `bash commands.sh` runs
everything together. Run `exit` after all experiments to close the transcript.

Then collect the files and transcript:

```bash
tar -czf ../lab12-results.tar.gz ./*.c commands.sh README.md assignment.pdf session.log
ls -lh ../lab12-results.tar.gz
```

The archive is in the parent directory. A transcript is useful when reproducing
the work. The submitted screenshots are retained in screenshots/.

## Interpreting the experiments

- The good producer/consumer should take items 1 through 8 in order, never
  exceed four occupied slots and finish with zero. Thread interleaving varies.
- The broken producer can hold the mutex while waiting for empty. When the
  buffer is full, the consumer needs that mutex to make room. A timeout exit
  code of 124 records that the process did not finish within the limit. If a
  run finishes, repeat it; the interleaving is scheduling dependent.
- The if version of condvar can appear correct in this small example. It
  does not demonstrate that if is safe: a spurious wakeup can occur while
  ready is still zero. The final source retains the while check.
- Reader and writer order can vary. The first reader takes room and the last
  releases it. This algorithm favors readers and can starve writers under a
  continuing stream of readers; these finite runs do not prove starvation.
- readers.c keeps its diagnostic printf inside rlock. The handout prints the
  changing readers counter after releasing rlock, which is a C data race.
  Only the diagnostic is moved; the simulated reading interval remains
  outside rlock, so readers can still overlap.
- Opposite lock order can deadlock even without sleeps. Removing sleeps
  changes the likelihood of the interleaving; ten successful runs do not
  establish correctness. The fixed source uses A then B in both threads.
- stdbuf keeps output visible when timeout kills a process during a logged
  or redirected run. Expected timeouts are handled so commands.sh continues.

## Final hand-in

Keep prodcons.c, condvar.c, readers.c and the fixed deadlock.c. The experiment
variants remain separate. The [Word report](../lab12_report.docx) contains all
Observe answers and the results recorded in the eight supplied screenshots.

## Recorded results

The experiments were performed on 15 November 2026 at 18:52:05 UTC in Ubuntu
Linux inside VirtualBox, as vboxuser on rustem. The working directory was
/home/vboxuser/os-lab12/lab12, the kernel was 7.0.0-38-generic, and GCC was
15.2.0 from Ubuntu package 15.2.0-16ubuntu1.

- Producer/consumer: items 1 through 8 consumed in order; maximum occupancy 4;
  final occupancy 0.
- Producer locking before sem_wait(empty): four puts followed by timeout 124.
  Running the original again completed with occupancy 0.
- Condition variable: the original while version, experimental if version and
  original again all read hello after the writer announced completion.
- Readers/writers: the first run had three readers reading 0 before the writers
  wrote 10 and 20. The next two runs had all readers reading 10 between the two
  writers. Maximum reported reader count was 3.
- Original opposite lock order with sleeps: timeout exit code 124.
- Without sleeps: runs 1, 2, 3, 5 and 9 completed with code 0; runs 4, 6, 7, 8
  and 10 timed out with code 124. The failure remained possible and became
  intermittent.
- Fixed lock order: both threads acquired A before B, printed completion,
  followed by done and exit code 0.

The nine-page Word report retains the Lab 11 cover, styles, page geometry,
captions and footer. The original screenshot bytes remain unchanged. A terminal
transcript was not supplied; the recorded results are taken from the screenshots.
