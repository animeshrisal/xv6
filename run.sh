qemu-system-riscv64 -machine virt -device virtio-gpu-device -smp 1 -bios none -kernel ./kernel -global virtio-mmio.force-legacy=false -m 512M

	qemu-system-riscv64 -nographic -machine virt -smp 1 -bios none -kernel kernel -device loader,addr=0x80200000,file=user1.bin -device loader,addr=0x80400000,file=user2.bin -device loader,addr=0x80600000,file=user3.bin
