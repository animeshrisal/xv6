#!/bin/bash
rm -rf *.o
rm -rf kernel.bin

riscv64-unknown-elf-gcc -g -mcmodel=medany -mno-relax -c boot.S
riscv64-unknown-elf-gcc -g -mcmodel=medany -mno-relax -c ex.S
riscv64-unknown-elf-gcc -g -mcmodel=medany -mno-relax -c gpu.S
riscv64-unknown-elf-gcc -g -mcmodel=medany -mno-relax -c setup.c
riscv64-unknown-elf-gcc -g -mcmodel=medany -mno-relax -c plic.c
riscv64-unknown-elf-gcc -g -mcmodel=medany -mno-relax -c trap.c
riscv64-unknown-elf-gcc -g -mcmodel=medany -mno-relax -c uart.c
riscv64-unknown-elf-gcc -g -mcmodel=medany -mno-relax -c tprintf.c
riscv64-unknown-elf-gcc -g -mcmodel=medany -mno-relax -c user_pong.c
riscv64-unknown-elf-gcc -g -mcmodel=medany -mno-relax -c talloc.c
riscv64-unknown-elf-gcc -g -mcmodel=medany -mno-relax -c display.c
riscv64-unknown-elf-gcc -g -mcmodel=medany -mno-relax -c syscall.c
riscv64-unknown-elf-gcc -g -mcmodel=medany -mno-relax -c kernel.c
riscv64-unknown-elf-gcc -g -mcmodel=medany -mno-relax -c main.c

riscv64-unknown-elf-gcc -g -ffreestanding -fno-common -nostdlib -mno-relax -mcmodel=medany   -Wl,-T kernel.ld *.o -o kernel
riscv64-unknown-elf-objcopy -O binary kernel kernel.bin
