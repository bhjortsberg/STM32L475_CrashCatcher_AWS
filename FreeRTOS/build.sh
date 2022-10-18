#!/bin/bash
set -e
if [ ! -d build ]; then
    cmake -DVENDOR=st -DBOARD=stm32l475_discovery -DCOMPILER=arm-gcc -S . -B build
fi
cmake --build build --parallel 8
arm-none-eabi-objcopy -O binary build/aws_demos.elf build/aws_demos.bin
JLink.exe con command.jlink
