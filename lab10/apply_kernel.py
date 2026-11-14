"""Apply Lab 10 Part 2 from the xv6 root, after the Part 1 checkpoint."""

from pathlib import Path


paths = ["kernel/proc.h", "kernel/proc.c", "kernel/sysproc.c", "kernel/syscall.c"]
contents = {name: Path(name).read_text() for name in paths}

if "trace_mask" in contents["kernel/proc.h"]:
    raise SystemExit("trace_mask already exists; do not apply Part 2 twice.")


def after_line(name, marker, addition):
    lines = [line for line in contents[name].splitlines(keepends=True) if marker in line]
    if len(lines) != 1:
        raise SystemExit(f"Expected one {marker!r} line in {name}; no files changed.")
    contents[name] = contents[name].replace(lines[0], lines[0] + addition, 1)


after_line("kernel/proc.h", "char name[16];", "  int trace_mask; // System calls selected for tracing\n")
after_line("kernel/proc.c", "p->state = USED;", "  p->trace_mask = 0;\n")
after_line(
    "kernel/proc.c",
    "safestrcpy(np->name, p->name, sizeof(p->name));",
    "  np->trace_mask = p->trace_mask;\n",
)

contents["kernel/sysproc.c"] += """
uint64
sys_trace(void)
{
  int mask;
  argint(0, &mask);
  myproc()->trace_mask = mask;
  return 0;
}
"""

after_line("kernel/syscall.c", "extern uint64 sys_sync(void);", "extern uint64 sys_trace(void);\n")
after_line("kernel/syscall.c", "[SYS_sync]", "  [SYS_trace] = sys_trace,\n")

names = [
    "fork", "exit", "wait", "pipe", "read", "kill", "exec", "fstat",
    "chdir", "dup", "getpid", "sbrk", "pause", "uptime", "open", "write",
    "mknod", "unlink", "link", "mkdir", "close", "sync", "trace",
]
table = "static char *syscallnames[] = {\n"
table += "".join(f'  [SYS_{name}] = "{name}",\n' for name in names)
table += "};\n\n"
anchor = "void\nsyscall(void)\n"
if contents["kernel/syscall.c"].count(anchor) != 1:
    raise SystemExit("Cannot locate syscall(); no files changed.")
contents["kernel/syscall.c"] = contents["kernel/syscall.c"].replace(anchor, table + anchor, 1)

after_line(
    "kernel/syscall.c",
    "p->trapframe->a0 = syscalls[num]();",
    '    if ((p->trace_mask >> num) & 1)\n'
    '      printk("%d: syscall %s -> %ld\\n",\n'
    '             p->pid, syscallnames[num], p->trapframe->a0);\n',
)

# Validate every insertion before changing any source file.
for name, text in contents.items():
    Path(name).write_text(text)
    print(f"Updated {name}")
