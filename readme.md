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

- Wifi credentials in `demos/include/aws_clientcredential.h`
- AWS Iot MQTT certificate and endpoint in `demos/include/aws_clientcredential.h`
  and `demos/include/aws_clientcredential_keys.h`

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

Load the image using [JLink commander](https://www.segger.com/downloads/jlink/)

    $ cd FreeRTOS
    $ JLinkExe con command.jlink

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

    $ aws s3 cp s3://<bucketname>/Crash/<thing-name>/<timestamp>/crash.dmp .

Dump the backtrace:

    $ gdb-multiarch build/aws_demos.elf -ex "set target-charset ASCII" -ex "target remote | ./CrashDebug --elf build/aws_demos.elf --dump crash.dmp" -quiet -ex "bt" -ex "echo Fault status registers\n" -ex "x/5xw 0xE000ED28" -ex "info registers" -ex "kill inferiors 1" -ex "quit"
    Reading symbols from build/aws_demos.elf...
    Remote debugging using | ./CrashDebug --elf build/aws_demos.elf --dump crash.dmp
    0x0badc0de in ?? ()
    #0  0x0badc0de in ?? ()
    #1  0x0800f142 in bhj_crash ()
    #2  0x0800f162 in moreCalls ()
    #3  0x0800f17a in anothercall ()
    #4  0x0800f192 in call1 ()
    #5  0x0800f238 in ButtonTask ()
    #6  0x0803b618 in pxPortInitialiseStack ()
    Backtrace stopped: Cannot access memory at address 0xa5a5a5b9
    Fault status registers
    0xe000ed28:     0x00000100      0x40000000      0x00000000      0xe000ed34
    0xe000ed38:     0xe000ed38
    r0             0x6                 6
    r1             0x100019e4          268442084
    r2             0x1000087c          268437628
    r3             0xbadc0de           195936478
    r4             0xa5a5a5a5          -1515870811
    r5             0xa5a5a5a5          -1515870811
    r6             0xa5a5a5a5          -1515870811
    r7             0x10002570          268445040
    r8             0xa5a5a5a5          -1515870811
    r9             0xa5a5a5a5          -1515870811
    r10            0xa5a5a5a5          -1515870811
    r11            0xa5a5a5a5          -1515870811
    r12            0x1000128c          268440204
    sp             0x10002570          0x10002570 <ucHeap2.14704+4928>
    lr             0x800f143           134279491
    pc             0xbadc0de           0xbadc0de
    xpsr           0x30000             196608
    fpscr          0x0                 0
    msp            0x20017fe0          536969184
    psp            0x10002508          268444936

If not doing `-ex "quit"` and `-ex "kill inferiors 1"` gdb will stay open and
can be used to inspect the dump.
