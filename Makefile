
# Build the kernel and user process binaries

CC=riscv64-unknown-elf-gcc
CFLAGS=-g -mcmodel=medany -mno-relax -I. -ffreestanding
OBJCOPY=riscv64-unknown-elf-objcopy

KERNELDEPS = hardware.h riscv.h types.h display.h plic.h proc.h talloc.h tprintf.h trap.h uart.h virtio.h kerneldef.h kernel.h param.h syscall.h gpu_driver.h spinlock.h cpu.h 
KERNELOBJS = boot.o kernel.o ex.o setup.o display.o plic.o proc.o syscall.o talloc.o tprintf.o trap.o uart.o gpu_driver.o spinlock.o cpu.o
USERDEPS = user_collision.h display.h types.h
USER_COLLISION_OBJS = user.o gpu.o user_collision.o uprintf.o 
USER_RNG_OBJS = user.o user_rng.o uprintf.o 
USER_PRINT_OBJS = user.o user_print.o uprintf.o

%.o: %.c $(KERNELDEPS) $(USERDEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: %.S $(KERNELDEPS) $(USERDEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all:  user_collision.bin kernel 

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

user_rng.bin: $(USER_RNG_OBJS) $(USERDEPS)
	$(CC) $(CFLAGS) -c user_rng.c
	$(CC) $(CFLAGS) -c user.S
	$(CC) -g -ffreestanding -fno-common -nostdlib -mno-relax \
	      -mcmodel=medany   -Wl,-T user.ld $(USER_RNG_OBJS) -o user_rng
	$(OBJCOPY) -O binary user_rng user_rng.bin

user_rng.bin: $(USER_RNG_OBJS) $(USERDEPS)
	$(CC) $(CFLAGS) -c user_rng.c
	$(CC) $(CFLAGS) -c user.S
	$(CC) -g -ffreestanding -fno-common -nostdlib -mno-relax \
	      -mcmodel=medany   -Wl,-T user.ld $(USER_RNG_OBJS) -o user_rng
	$(OBJCOPY) -O binary user_rng user_rng.bin

user_print.bin: $(USER_PRINT_OBJS) $(USERDEPS)
	$(CC) $(CFLAGS) -c user_print.c
	$(CC) $(CFLAGS) -c user.S
	$(CC) -g -ffreestanding -fno-common -nostdlib -mno-relax \
	      -mcmodel=medany   -Wl,-T user.ld $(USER_PRINT_OBJS) -o user_print
	$(OBJCOPY) -O binary user_print user_print.bin



run: user_collision.bin user_rng.bin user_print.bin kernel 
	qemu-system-riscv64 -machine virt -device virtio-gpu-device -smp 2 -bios none -kernel ./kernel -global virtio-mmio.force-legacy=false -device virtio-rng-device -device loader,addr=0x81000000,file=user_collision.bin  -device loader,addr=0x82000000,file=user_rng.bin

	
clean:
	-@rm -f *.o *.bin kernel user_collision user_rng user_print user
