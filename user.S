.section .text
.global _entry

_entry:
  la sp, userstack
  li a0, 4096
  add sp, sp, a0
  

  jal main

loop:
  j loop


