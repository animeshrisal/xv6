
.globl ex
.globl exception
.align 4
ex:
	      csrrw a0, mscratch, a0
        addi sp, sp, -256
	      add sp, sp, a0

	      csrrw a0, mscratch, a0

        sd ra, 0(sp)
        sd sp, 8(sp)
        sd gp, 16(sp)
        sd tp, 24(sp)
        sd t0, 32(sp)
        sd t1, 40(sp)
        sd t2, 48(sp)
        sd s0, 56(sp)
        sd s1, 64(sp)
        sd a0, 72(sp)
        sd a1, 80(sp)
        sd a2, 88(sp)
        sd a3, 96(sp)
        sd a4, 104(sp)
        sd a5, 112(sp)
        sd a6, 120(sp)
        sd a7, 128(sp)
        sd s2, 136(sp)
        sd s3, 144(sp)
        sd s4, 152(sp)
        sd s5, 160(sp)
        sd s6, 168(sp)
        sd s7, 176(sp)
        sd s8, 184(sp)
        sd s9, 192(sp)
        sd s10, 200(sp)
        sd s11, 208(sp)
        sd t3, 216(sp)
        sd t4, 224(sp)
        sd t5, 232(sp)
        sd t6, 240(sp)

	mv a0, sp


        call kernel_trap

        ld ra, 0(a0)
        ld sp, 8(a0)
        ld gp, 16(a0)
        // ld tp, 24(a0) in case we change cpus
        ld t0, 32(a0)
        ld t1, 40(a0)
        ld t2, 48(a0)
        ld s0, 56(a0)
        ld s1, 64(a0)
        ld a1, 80(a0)
        ld a2, 88(a0)
        ld a3, 96(a0)
        ld a4, 104(a0)
        ld a5, 112(a0)
        ld a6, 120(a0)
        ld a7, 128(a0)
        ld s2, 136(a0)
        ld s3, 144(a0)
        ld s4, 152(a0)
        ld s5, 160(a0)
        ld s6, 168(a0)
        ld s7, 176(a0)
        ld s8, 184(a0)
        ld s9, 192(a0)
        ld s10, 200(a0)
        ld s11, 208(a0)
        ld t3, 216(a0)
        ld t4, 224(a0)
        ld t5, 232(a0)
        ld t6, 240(a0)
        ld a0, 72(a0) // has to be done last...

        addi sp, sp, 256

        // Restore the original SP

        // Load the PA base into a0 and store a0 in mscratch
        csrrw a0, mscratch, a0
        // Adjust the SP to point to the PA now
        sub sp, sp, a0
        // Swap PA base and a0 back so we have the original value 
        // in a0 again
        csrrw a0, mscratch, a0

        // make room to save registers.
        // return to whatever we were doing in the kernel.
        mret

