# Lab 11 The Race and the Semaphore

Cimendur Rustem, FAF-243.

These Linux programs follow the supplied Lab 11 handout. Run them in Ubuntu
Linux, as in the earlier Track A laboratories. They do not run inside xv6.
Compile with `-pthread` and without optimization flags.

| File | Experiment |
| --- | --- |
| race.c | Two threads increment an unprotected shared counter. |
| mutex.c | A mutex protects each increment. |
| sem.c | A semaphore initialized to one protects each increment. |
| order.c | A semaphore initialized to zero signals that a message is ready. |
| parking.c | A counting semaphore limits parking to three cars. |
| atomic.c | Optional atomic increment experiment. |
| sem_two.c | Experimental semaphore initialized to two permits. |
| order_one.c | Experimental readiness semaphore initialized to one. |
| parking_one.c | Experimental parking capacity of one. |
| commands.sh | Commands for every required experiment and the optional comparison. |
| assignment.pdf | Original eight-page laboratory handout. |
| report.md | Recorded results and all Observe answers. |
| screenshots/ | Six original execution screenshots. |

Run the commands in `commands.sh` in sections and capture the command and output
for each section. Keep the final source versions unchanged; `sed` creates
separate files for the deliberately changed experiments.

Record the environment and terminal session before running the experiments:

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

After the final experiment, `exit` finishes the recording.

The race examples intentionally contain unsynchronized access. A lost update
is a useful way to explain the observed output, but the C program has a data
race and therefore undefined behavior. A run can happen to print 2000000;
repeat it to look for the race. The semaphore initialized to two likewise
does not provide mutual exclusion. Measured timings and their ordering depend
on the machine, scheduling and virtualization.

## Recorded results

The experiments were performed on 15 November 2026 in Ubuntu Linux inside
VirtualBox, as vboxuser on rustem. The working directory was
`/home/vboxuser/os-lab11/lab11`, the kernel was `7.0.0-38-generic`, and GCC
was `15.2.0` from the Ubuntu `15.2.0-16ubuntu1` package.

- Unprotected counter: 1231830, 1022067 and 1040272.
- Mutex: 2000000 in all three runs.
- Semaphore initialized to one: 2000000 in all three runs.
- Semaphore initialized to two: 1793321, 1733887 and 1788827.
- Readiness initialized to zero: B reads hello after A writes it.
- Readiness initialized to one: B reads an empty string before A writes.
- Three parking places: maximum occupancy 3; final occupancy 0.
- One parking place: occupancy alternates between 1 and 0.
- Optional atomic counter: 2000000.

| Program | Real seconds | User seconds | System seconds |
| --- | --- | --- | --- |
| race | 0.009 | 0.008 | 0.003 |
| mutex | 0.076 | 0.075 | 0.059 |
| sem | 0.123 | 0.072 | 0.152 |
| atomic | 0.018 | 0.030 | 0.001 |

The Word report is `../lab11_report.docx`. It follows the Lab 10 report's
cover, styles, page geometry and captions. Its screenshot excerpts are cropped
only to separate experiments; the original images remain in `screenshots/`.
The three variant sources reproduce the one-line diffs shown in the screenshots.
