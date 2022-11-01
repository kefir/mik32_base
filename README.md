# MIK32 base firmware

Testing ground for MIK32 core and peripherals.

Current code runs in RAM for ease of debugging.

## Cloning

Just remember to run ```git submodule update``` as [Mikron BSP library (fork)](https://github.com/kefir/mik32-shared/tree/develop) is added to the project as submodule.

## Building

Target elf file is built by running.

```bash
sh build.sh
```

Shell script is used instead of full build system for ease of use and transparency.
If your compiler is different from the one used in the script - edit PREFIX in **build.sh** script:

```bash
PREFIX=riscv32-unknown-elf
```

## Toolchain details

***GCC 11.1.0 (riscv32-unknown-elf):***
```
Using built-in specs.
COLLECT_GCC=riscv32-unknown-elf-gcc
COLLECT_LTO_WRAPPER=/usr/local/libexec/gcc/riscv32-unknown-elf/11.1.0/lto-wrapper
Target: riscv32-unknown-elf
Configured with: configure --target=riscv32-unknown-elf --prefix=/usr/local --disable-shared --disable-threads --enable-languages=c,c++ --with-pkgversion=g5964b5cd727 --with-system-zlib --enable-tls --with-newlib --with-sysroot=/usr/local/riscv32-unknown-elf --with-native-system-header-dir=/include --disable-libmudflap --disable-libssp --disable-libquadmath --disable-libgomp --disable-nls --disable-tm-clone-registry --src=../../riscv-gcc --disable-multilib --with-abi=ilp32 --with-arch=rv32imac --with-tune=rocket --with-isa-spec=2.2 'CFLAGS_FOR_TARGET=-Os   -mcmodel=medlow' 'CXXFLAGS_FOR_TARGET=-Os   -mcmodel=medlow'
Thread model: single
Supported LTO compression algorithms: zlib zstd
gcc version 11.1.0 (g5964b5cd727)
```

***GNU 12.1 (riscv32-elf-gdb):***
```
GNU gdb (GDB) 12.1
Copyright (C) 2022 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
```
***OpenOCD 0.11.0 (Syntacore):***
```
Open On-Chip Debugger 0.11.0+dev-02353-g49cf334e9 (2022-10-13-16:37)
```

## Contributing

**Code style:** PEP8-like, GTK-like

**Formatting:** clang-format WebKit default ruleset