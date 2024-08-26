#!/bin/bash
riscv64-unknown-elf-gcc -g -mcmodel=medany -mno-relax -c boot.S
riscv64-unknown-elf-gcc -g -mcmodel=medany -mno-relax -c ex.S
riscv64-unknown-elf-gcc -g -mcmodel=medany -mno-relax -c setup.c
riscv64-unknown-elf-gcc -g -mcmodel=medany -mno-relax -c kernel.c
riscv64-unknown-elf-gcc -g -mcmodel=medany -mno-relax -c user.c
riscv64-unknown-elf-gcc -g -mcmodel=medany -mno-relax -c display.c
riscv64-unknown-elf-gcc -g -mcmodel=medany -mno-relax -c virtq.c

riscv64-unknown-elf-gcc -g -mcmodel=medany -mno-relax -c main.c
riscv64-unknown-elf-gcc -g -ffreestanding -fno-common -nostdlib -mno-relax -mcmodel=medany   -Wl,-T kernel.ld *.o -o hello
riscv64-unknown-elf-objcopy -O binary hello hello.bin
