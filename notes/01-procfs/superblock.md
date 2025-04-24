## Superblock

In Linux VSD, a superblock is a critical data structure that represents a mounted filesystem. It contains metadata like:

- Filesystem type (e.g., ext4, procfs, ntfs)
- Block size adn maximum file size
- Location of the root [inode](inode.md)
- Pointers to filesystem-specific operations via `super_operations`

Each mounted filesystem has its own `struct super_block` instance. In case of `procs`, it is dynamically created at boot time and isn't saved on physical storage.