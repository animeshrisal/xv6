.section .text
.global _entry
_entry:
  // load stack0 address into stack pointer
  la sp, stack0

  // load 4096 value into a0
  li a0, 1024*4

  csrr a1, mhartid
  addi a1, a1, 1
  mul a0, a0, a1
  add sp, sp, a0

  call start

spin:
  j spin
