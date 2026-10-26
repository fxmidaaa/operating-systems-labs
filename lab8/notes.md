# Lab 8 Observe answers

Student: Cimendur Rustem, FAF-243

## Part 1

1. The xv6 program uses `fork()`, `exec()`, and `wait()`.
   The Linux Lab 7 program used `fork()`, `execlp()`, and `waitpid()`.
   Both follow the same process lifecycle: create a child,
   replace its program, and collect its termination status.
   The interfaces differ: xv6 `exec` takes an argument array,
   and `wait` collects a child without selecting a PID.

2. The `echo` program prints "hello from the child".
   After a successful `exec`, the child runs `echo` instead of `run`.
   The `run` program prints only its parent and child diagnostics.

3. The xv6 failure branch explicitly calls `exit(1)`.
   The Linux Lab 7 failure branch explicitly called `exit(127)`.
   These statuses were chosen by the programs; `exec` itself
   does not automatically assign either exit status.

## Part 2

1. Each pipe provides one byte stream with a read end and a
   write end. Two pipes provide separate directions: `p2c` carries
   the parent's byte to the child, and `c2p` carries the reply.
   Sharing one stream in both directions would let a process
   consume its own message and would not reliably route replies.

2. The parent's `read(c2p[0], &b, 1)` blocks while the reply pipe
   is empty and its write end remains open. The child writes
   the reply after printing ping, allowing the parent to read
   the byte and print pong. The later `wait(0)` collects the
   child's termination; it does not perform the byte exchange.
