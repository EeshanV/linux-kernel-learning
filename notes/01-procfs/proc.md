## What is `/proc`?

`/proc` is a **pseudo-filesystem** that the Kernel dynamically creates at boot to expose the internal data structures.

Linux's procfs lets us inspect process info (`/proc/[PID]/*`), CPU stats (`/proc/cpuinfo`), memory maps (`/proc/[PID]/maps`), etc.

## How `/proc` works?

### VFS Integration

- At boot, the kernel registers procsf as a special **VFS** type. It implements its own [superblock](superblock.md) and [inode](inode.md)
- No underlying block is needed, every file is handled by kernel callbacks that populate the buffers dynamically.

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

## Nested Directories

Create a directory under `/proc` by:

```c

struct proc_dir_entry *parent;
parent = proc_mkdir("mydriver", NULL);
proc_create("stats", 0444, parent, &my_ops);
```

## Reading via `seq_file` interface

Use `seq_file` helpers to:
- Open: `.proc_open = my_open` where `my_open()` calls `single_open(file, show_fn, data_ptr)`
- Show: `show_fn(struct seq_file *s, void *v)` uses `seq_printf(s, "...")` to write pages of data
- Read/Lseek: `seq_read` and `seq_lseek` are wired in automatically
- Release: `.proc_release = single_release` frees the iterator

## Writing to procfs files

To accept input from userspace, add a `.proc_write` callback in your `proc_ops`

```c

static ssize_t my_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos) {
  char kbuf[128];
  if (count > sizeof(kbuf) -1)
    return -EINVAL;
  if (copy_from_user(kbuf, buf, count))
    return -EFAULT;
  kbuf[count] = '\0';
  retunr count;
}

struct proc_ops my_ops = {
  /* ... */
  .proc_write = my_write,
};
```

## Cleaning up

```c

remove_proc_entry("stats", parent);
remove_proc_entry("mydriver", NULL);
```

## Exposing [Tunables](tunables.md) via `/proc/sys` (sysctl)

The sysctl interface under `/proc/sys` offers a structured way to expose kernel parameters:

```c

static struct ctl_table my_table[] = {
  {
  .procname = 'foo_enabled',
  .data = &my_foo_enabled,
  .maxlen = sizeof(int),
  .proc_handler = proc_dointvec
  },
  { }
};

static struct ctl_table_header *hdr;

hdr = register_sysctl("kernel/my_module", my_table);
/* ... */
unregister_sysctl_table(hdr);
```

This creates `/proc/sys/kernel/my_module/foo_enable` that users can `echo 1 > …` to toggle features at runtime