## Inode

An **inode** (index node) is a data structure that represents the filesystem object, such as a file or directory
Each inode contains:
- File type and permissions
- Owner and group information
- File size
- Timestamps (creation, modification, access)
- Pointers to data blocks

In VFS, `struct inode` abstracts these details, allowing the kernel to interact with different filesystem types uniformly.
For `procfs`, inodes are generated on-the-fly to represent virtual files that provide information about the system and the processes.