## What is a Tunable?

Tunable (or kernel parameter) is a variable inside the kernel that you can adjust during runtime to infulence its subsystem.
Modifying a tunable is as quick as writing a new value to its corresponding `/proc/sys/...` file or using `sysctl` utility.


## `/proc/sys` Interface

- `/proc/sys` is mounted automatically by the kernel and contains subdirectories for major subsystem (e.g., `kernel/`, `net/`, `vm/`)
- Each file under `/proc/sys/<subsystem>` maps directly to a kernel variable. For example `/proc/sys/kernel/panic` controls how long the kernel waits before rebooting on a panic
- Reading a file (e.g., `cat /proc/sys/vm/swappiness`) returns the current value. Writing updates the variable immediately (`echo 10 > /proc/sys/vm/swappiness`)

## Programmatic Tunables

Kernel modules can register new tunables by defining an array of `struct ctl_table` entries, each specifing:
- `procname`: the file under `/proc/sys/...`
- `data`: pointer to actual kernel variables
- `maxlen`: its size in bytes
- `mode`: filesystem permission
- `proc_handler`: a callback (e.g., `proc_dointvec()`) to parse and validate user input

calling `register_sysctl("kernel/my_module", my_table)` mounts the table under `/proc/sys/kernel/my_module` and returns a header you must pass later to `unregister_sysctl_table()` in your module's exit path.

The kernel provides default handlers for common types: `proc_dointvec()`, `proc_dostring()`, `proc_dointvec_minmax()`, etc., simplifying most use cases.

## Listying and Modifying Tunables using `sysctl`

- listing all tunables:
```bash
sysctl -a
```
shows every parameter in the `/proc/sys/` tree

- reading a specific tunable:
```bash
sysctl net.ipv4.ip_forward
```
outputs its current setting (`0` or `1`)

- writing temporarily:
```bash
sysctl vm.swappiness=20
```
updates kernel immediately, but the change is lost on reboot unless persisted
