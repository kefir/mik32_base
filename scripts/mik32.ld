
OUTPUT_FORMAT("elf32-littleriscv")
OUTPUT_ARCH(riscv)

ENTRY(_start)

MEMORY {
  TCM (rwx) : ORIGIN = 0x00000000, LENGTH = 16K
}

SECTIONS {
    .text : {
        
    } >TCM
}
