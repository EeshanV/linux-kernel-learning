## Core Modules

KVM has two built-in modules: `kvm.o` (generic core) and either `kvm-intel.ko` or `kvm-amd.ko` (architecture-specific accelerator)

- `kvm.ko` registers `/dev/kvm` character device and implements the ioctl dispatch logic for creating and controlling VMs

- `kvm-intel.ko` or `kvm-amd.ko` modules hook into the hardware virtualization extensions (Intel VT-x or AMD-V) to manage VMCS (VM control Structures) or VMCB (VM Control Block) and enable features like EPT or NPT


## CPU Virtualization Extentions

- Intel VT-x provides VM entry/exit instructions, VMCS for state storage, and EPT (Extended Page Tables) for second-level address translation

- AMD-V (SVM) offeres equivalent functionality via VMBC and NPT (Nested Page Tables) to map guest-physical to  host-physical memory

## `/dev/kvm` API & Lifecycle

### Opening `/dev/kvm` & creating VMs

- Open `/dev/kvm` to get a KVM fd

```c
int kvm_fd = open("/dev/kvm", O_RDWR | O_CLOEXEC);
```

- Create a VM with `KVM_CREATE_VM` ioctl on kvm fd, returning a VM fd

```c
int vm_fd = ioctl(kvm_fd, KVM_CREATE_VM, vcpu_index);
```

- Create vCPUs via `KVM_CREATE_VCPU` on VM fd, returning a VCPU fd for each virtual core

```c
int vcpu_fd = ioctl(vm_fd, KVM_CREATE_VCPU, vcpu_index);
```

### Setting up Guest Memory

- Allocate a userspace buffer for guest RAM , then map it into the VM using `KVM_SET_USER_MEMORY`

```c
struct kvm_userspace_memory_region region = {
  .slot = 0,
  .guest_phys_addr = 0x100000,
  .memory_size = mem_size,
  .userspace_addr = (uint64_t)host_mem,
};
ioctl(vm_fd, KVM_SET_USER_MEMORY)
```