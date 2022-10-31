
#define EXCEPTION_STACK_SPACE 34*4
#define EXCEPTION_SAVED_REGISTERS 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31

.equ STACK_TOP, 0x00004000

# Save caller registers
.macro _SAVE_CONTEXT
    .irp index, EXCEPTION_SAVED_REGISTERS
        sw      x\index, 4*\index(sp)
    .endr
.endm

# restore caller registers
.macro _RESTORE_CONTEXT
    .irp index, EXCEPTION_SAVED_REGISTERS
        lw      x\index, 4*\index(sp)
    .endr
.endm

# Save the mepc and mstatus
.macro _SAVE_EPC_STATUS
    csrr x5, mepc  
    sw   x5, 128(sp)
    csrr x5, mstatus
    sw   x5, 132(sp)
.endm

# Restore the mepc and mstatus
.macro _RESTORE_EPC_STATUS
  lw   x5, 128(sp)
  csrw mepc, x5  
  lw   x5, 132(sp)
  csrw mstatus, x5  
.endm

.text

.global _start
.type   _start, @function

_start:
    .option push                    # 'Save' options state
    .option norelax                 # Tell linker not to optimize memory

        la gp, __global_pointer$
        la sp, STACK_TOP

    .option pop
    

    # Clear the bss segment
    la  a0, __bss_start      # a0 = start zeroing memory here
    la  a1, __BSS_END__      # a1 = end zeroing here-1

    clear_bss:
        bgeu    a0, a1, finish_bss      # while (!(a0 >= a1))
        sb      x0, 0(a0)             #   *a0 = 0;
        addi    a0, a0, 1             #   ++a0
        beq     x0, x0, clear_bss

    finish_bss:
        call main

    spin:
	    wfi
        j   spin

.size _start, .-_start


.org 0xC6
.global _trap_handler
.type   _trap_handler, @function

_trap_handler:
.option norelax
    addi    sp, sp, -(EXCEPTION_STACK_SPACE)

    _SAVE_CONTEXT
    # _SAVE_EPC_STATUS

    call trap_handler_default

    # _RESTORE_EPC_STATUS
    _RESTORE_CONTEXT

    addi    sp, sp, EXCEPTION_STACK_SPACE
    mret

# .space 2
.size _trap_handler, .-_trap_handler
