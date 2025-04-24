## What is a Tunable?

Tunable (or kernel parameter) is a variable inside the kernel that you can adjust during runtime to infulence its subsystem.
Modifying a tunable is as quick as writing a new value to its corresponding `/proc/sys/...` file or using `sysctl` utility.


## `/proc/sys` Interface

- `/proc/sys` is mounted automatically by the kernel and contains subdirectories for major subsystem (e.g., `kernel/`, `net/`, `vm/`)
- Each file under `/proc/sys/<subsystem>` maps directly to a kernel variable. For example `/proc/sys/kernel/panic` controls how long the kernel waits before rebooting on a panic
- Reading a file (e.g., `cat /proc/sys/vm/swappiness`) returns the current value. Writing updates the variable immediately (`echo 10 > /proc/sys/vm/swappiness`)

