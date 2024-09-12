
# Build the kernel and user process binaries

CC=riscv64-unknown-elf-gcc
CFLAGS=-g -mcmodel=medany -mno-relax -I. -ffreestanding
OBJCOPY=riscv64-unknown-elf-objcopy

KERNELDEPS = hardware.h riscv.h types.h display.h plic.h proc.h talloc.h tprintf.h trap.h uart.h virtio.h kerneldef.h kernel.h param.h syscall.h 
KERNELOBJS = boot.o kernel.o ex.o setup.o display.o main.o plic.o proc.o syscall.o talloc.o tprintf.o trap.o uart.o
USERDEPS = user_collision.h display.h types.h  
USER_COLLISION_OBJS = gpu.o user_collision.o user.o 


%.o: %.c $(KERNELDEPS) $(USERDEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: %.S $(KERNELDEPS) $(USERDEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all:   kernel user_collision.bin

kernel: $(KERNELOBJS) $(KERNELDEPS)
	$(CC) -g -ffreestanding -fno-common -nostdlib -mno-relax \
	      -mcmodel=medany -Wl,-T kernel.ld $(KERNELOBJS) -o kernel
	$(OBJCOPY) -O binary kernel kernel.bin

user_collision.bin: $(USER_COLLISION_OBJS) $(USERDEPS)
	$(CC) $(CFLAGS) -c user_collision.c
	$(CC) $(CFLAGS) -c user.S
	$(CC) -g -ffreestanding -fno-common -nostdlib -mno-relax \
	      -mcmodel=medany   -Wl,-T user.ld $(USER_COLLISION_OBJS) -o user_collision
	$(OBJCOPY) -O binary user_collision user_collision.bin


run: 
	qemu-system-riscv64 -machine virt -device virtio-gpu-device -smp 1 -bios none -kernel ./kernel -global virtio-mmio.force-legacy=false -m 512M -device loader,addr=0xA00000000,file=user_collision.bin  
	
clean:
	-@rm -f *.o *.bin kernel user_collision user
