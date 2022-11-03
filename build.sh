#!/bin/sh


TARGET=mik32_base
BUILD_DIR=./build
DRIVERS_DIR=./drivers

PREFIX=riscv32-unknown-elf
LD=$PREFIX-ld
CC=$PREFIX-gcc
ASM=$PREFIX-as
OBJCOPY=$PREFIX-objcopy
OBJDUMP=$PREFIX-objdump
SIZE=$PREFIX-size

LD_SCRIPT="$DRIVERS_DIR/mik32/ldscripts/eeprom.ld"

FLAGS="
    -g -Wall -Wextra -ggdb3
    -march=rv32imc -mabi=ilp32
    -static -nostdlib -nostartfiles
    -fdata-sections -ffunction-sections
    -Wl,--gc-sections,--print-memory-usage
"

INCLUDES="
    -I./include
    -I$DRIVERS_DIR/mik32/include
    -I$DRIVERS_DIR/mik32/periphery/include
"

SOURCES="
    $DRIVERS_DIR/mik32/runtime/crt0.S
    target.c
    timers.c
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

