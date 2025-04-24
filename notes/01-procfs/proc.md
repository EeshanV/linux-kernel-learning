## What is `/proc`?

`/proc` is a **pseudo-filesystem** that the Kernel dynamically creates at boot to expose the internal data structures.

Linux's procfs lets us inspect process info (`/proc/[PID]/*`), CPU stats (`/proc/cpuinfo`), memory maps (`/proc/[PID]/maps`), etc.

## How `/proc` works?

### VFS Integration

- At boot, the kernel registers procsf as a special **VFS** type. It implements its own [superblock](superblock.md) and [inode](inode.md)
- No underlying block is needed, every file is handled by kernel callbacks that populate the bufferes dynamically.

### Core data structures

-  `struct proc_dir_create` represents each `/proc` file or directory and holds pointers to callbacks for open/read/write
-  `struct proc_ops` are used to clearly distinguish procfs hooks

When a file system is mounted, the kernel reads its superblock to understand the filesystem's structure. The superblock then provides access to the inode, which represent the individual files and directories.

## Creating procfs entries

To simply add file under `/proc`, use `proc_create()`

```c

struct proc_ops my_ops = {
  .proc_open = my_open,
  .proc_read = seq_read,
  .proc_lseek = seq_lseek,
  .proc_release = single_release,
};

proc_create("my_entry", 0644, NULL, &my_ops);
```

In the above code, `my_entry` becomes `/proc/my_entry` with permissions `0644`. Internally this allocates `struct proc_dir_entry` tied into VFS's superblock and inode machinery