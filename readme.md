# Send STM32L475 Crash dumps to AWS

This is a demo project where crash dumps are made using [CrashCatcher](https://github.com/adamgreen/CrashCatcher)
and and sent via MQTT to AWS where the crash dump can be stored in an AWS bucket.

The project is made for STM32 Discovery Kit Iot node using the STM32L475VG MCU
and AmazonFreeRTOS.

# How it works

In the demo there is a task that will crash when the button on the board is
pressed. When the crash occurs the hard fault handler will invoke CrashCatcher
which in turn will write the crash dump to flash. When the board is rebooted
the dump will be read from flash and sent to AWS over MQTT.

To use this demo one need to configure

- Wifi credentials in `aws_clientcredentials.h`
- AWS Iot MQTT certificate and endpoint in `aws_clientcredentials.h` and
  `aws_clientcredentials_keys.h`

Instructions are provided below.

# Prerequisite

- GCC cross compiler for ARM Cortex-R/M processors
  `$ sudo apt install gcc-arm-none-eabi`
- cmake
- AWS Account

# Build image

First create build environment

    $ cd FreeRTOS
    $ cmake -DVENDOR=st -DBOARD=stm32l475_discovery -DCOMPILER=arm-gcc -S . -B build

Start the actual build

    $ cmake --build build --parallel 8

Only the last cmake command is necessary when rebuilding.

## Create bin-file

    $ arm-none-eabi-objcopy -O binary build/aws_demos.elf build/aws_demos.bin

# Load image

Load the image using JLink commander

    $ cd FreeRTOS
    $ JLink con command.jlink

There is also a build script that will build and load the image:

    $ ./build.sh

# Setup AWS for storing crash dump in S3

## Create AWS Iot Thing

TBD

## Create AWS Iot Rule

TBD

## Iot Thing Certificate

TBD

# CrashCatcher and CrashDebug

In order to decode the crash dump use [CrashDebug](https://github.com/adamgreen/CrashDebug).

Fetch the crash dump from AWS S3 using AWS Cli (or use AWS Console):

    $ aws cp s3://<bucketname>/<prefixkey>/crash.dmp .

Dump the backtrace:

    $ gdb-multiarch build/aws_demos.elf -ex "set target-charset ASCII" -ex "target remote | ./CrashDebug --elf build/aws_demos.elf --dump crash.dmp" -quiet -ex "bt" -ex "echo Fault status registers\n" -ex "x/5xw 0xE000ED28" -ex "info registers" -ex "kill inferiors 1" -ex "quit"
    Reading symbols from build/aws_demos.elf...
    Remote debugging using | ./CrashDebug --elf build/aws_demos.elf --dump crash.dmp
    0x0badc0de in ?? ()
    #0  0x0badc0de in ?? ()
    #1  0x0800f3ca in bhj_crash ()
    Backtrace stopped: Cannot access memory at address 0x1000258c
    A debugging session is active.

            Inferior 1 [Remote target] will be killed.

If not doing `-ex "quit"` and `-ex "kill inferiors 1"` gdb will stay open and
can be used to inspect the dump.
