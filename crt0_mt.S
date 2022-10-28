#define EXCEPTION_STACK_SPACE 32*4
#define EXCEPTION_SAVED_REGISTERS 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31


.text

.global _start
.type   _start, @function

_start:
    .option push                    # 'Save' options state
    .option norelax                 # Tell linker not to optimize memory
        la gp, __global_pointer$
        la sp, _end
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
        call   main

    spin:
	    wfi
        j   spin

.size _start, .-_start

.option norvc
.org 0xC2
.global _trap_handler
.type   _trap_handler, @function

.word trap_handler_default

_trap_handler:
    call trap_handler_default
    mret

.size _trap_handler, .-_trap_handler
