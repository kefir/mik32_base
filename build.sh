#!/bin/sh


TARGET=mik32_base
BUILD_DIR=./build
DRIVERS_DIR=./drivers
LIBS_DIR=./lib
TARGET_DIR=./target

PREFIX=riscv32-unknown-elf
LD=$PREFIX-ld
CC=$PREFIX-gcc
ASM=$PREFIX-as
OBJCOPY=$PREFIX-objcopy
OBJDUMP=$PREFIX-objdump
SIZE=$PREFIX-size

LD_SCRIPT="$DRIVERS_DIR/mik32/ldscripts/ram.ld"

FLAGS="
    -g -Wall -Wextra -ggdb3 -O1
    -march=rv32imc -mabi=ilp32
    -static -nostdlib -nostartfiles
    -fdata-sections -ffunction-sections
    -Wl,--gc-sections,--print-memory-usage
"

INCLUDES="
    -I./include
    -I$DRIVERS_DIR/mik32/include
    -I$DRIVERS_DIR/mik32/periphery/include
    
    -I$TARGET_DIR/include

    -I$LIBS_DIR/esch/include
    -I$LIBS_DIR/mcp2512/include

"

SOURCES="
    $DRIVERS_DIR/mik32/runtime/crt0.S

    $LIBS_DIR/esch/esch_queue.c
    $LIBS_DIR/esch/esch_semaphore.c
    $LIBS_DIR/esch/esch.c
    $LIBS_DIR/esch/ports/template/esch_port_template.c

    $LIBS_DIR/mcp2512/mcp2515.c

    $TARGET_DIR/target.c
    $TARGET_DIR/timers.c
    $TARGET_DIR/spi.c

    main.c

"

echo "Cleaning..."
rm -fr $BUILD_DIR
mkdir $BUILD_DIR


echo "Compiling..."
$CC $FLAGS $INCLUDES -T$LD_SCRIPT $SOURCES -o $BUILD_DIR/$TARGET.elf
$OBJCOPY -Oihex $BUILD_DIR/$TARGET.elf $BUILD_DIR/$TARGET.hex

echo "Dumping asm..."
$OBJDUMP -Mnumeric,no-aliases -da $BUILD_DIR/$TARGET.elf > $BUILD_DIR/$TARGET.dump

echo "Listing symbols..."
nm $BUILD_DIR/$TARGET.elf | sort > $BUILD_DIR/$TARGET.sym

