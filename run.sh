qemu-system-riscv64 -machine virt -device virtio-gpu-device -smp 1 -bios none -kernel ./kernel -global virtio-mmio.force-legacy=false


