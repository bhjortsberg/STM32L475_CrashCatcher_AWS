cmake -DVENDOR=st -DBOARD=stm32l475_discovery -DCOMPILER=arm-gcc -S . -B build
cmake --build build --parallel 8
arm-none-eabi-objcopy -O binary build/aws_demos.elf build/aws_demos.bin
$ JLink.exe
SEGGER J-Link Commander V6.96 (Compiled Feb 19 2021 09:57:05)
DLL version V6.96, compiled Feb 19 2021 09:55:51

Connecting to J-Link via USB...O.K.
Firmware: J-Link STLink V21 compiled Aug 12 2019 10:29:20
Hardware version: V1.00
S/N: 776441727
VTref=3.300V


Type connect to establish a target connection, '?' for help
J-Link>connect
Please specify device / core. <Default>: STM32L475VG
Type '?' for selection dialog
Device>
Please specify target interface:
  J) JTAG (Default)
  S) SWD
  T) cJTAG
TIF>S
Specify target interface speed [kHz]. <Default>: 4000 kHz
Speed>
Device STM32L475VG selected.


Connecting to target via SWD
Found SW-DP with ID 0x4BA01477
Found SW-DP with ID 0x4BA01477
DPv0 detected
Scanning AP map to find all available APs
AP[1]: Stopped AP scan as end of AP map has been reached
AP[0]: AHB-AP (IDR: 0x24770011)
Iterating through AP map to find AHB-AP to use
AP[0]: Core found
AP[0]: AHB-AP ROM base: 0xE00FF000
CPUID register: 0x410FC241. Implementer code: 0x41 (ARM)
Found Cortex-M4 r0p1, Little endian.
FPUnit: 6 code (BP) slots and 2 literal slots
CoreSight components:
ROMTbl[0] @ E00FF000
ROMTbl[0][0]: E000E000, CID: B105E00D, PID: 000BB00C SCS-M7
ROMTbl[0][1]: E0001000, CID: B105E00D, PID: 003BB002 DWT
ROMTbl[0][2]: E0002000, CID: B105E00D, PID: 002BB003 FPB
ROMTbl[0][3]: E0000000, CID: B105E00D, PID: 003BB001 ITM
ROMTbl[0][4]: E0040000, CID: B105900D, PID: 000BB9A1 TPIU
ROMTbl[0][5]: E0041000, CID: B105900D, PID: 000BB925 ETM
Cortex-M4 identified.
J-Link>loadbin build/aws_demos.bin 0x8000000
Halting CPU for downloading file.
Downloading file [build/aws_demos.bin]...
J-Link: Flash download: Bank 0 @ 0x08000000: 1 range affected (301056 bytes)
J-Link: Flash download: Total: 6.667s (Prepare: 0.062s, Compare: 0.058s, Erase: 3.262s, Program & Verify: 3.278s, Restore: 0.004s)
J-Link: Flash download: Program & Verify speed: 89 KB/s
O.K.

# CrashCatcher and CrashDebug

Save the hexdump from the serial console into a textfile called crash.hex.dmp

Dump the backtrace:

    $ gdb-multiarch build/aws_demos.elf -ex "set target-charset ASCII" -ex "target remote | ~/devel/CrashDebug/bins/lin64/CrashDebug --elf build/aws_demos.elf --dump ../crash.hex.dmp" -quiet -ex "bt" -ex "quit"
    Reading symbols from build/aws_demos.elf...
    Remote debugging using | ~/devel/CrashDebug/bins/lin64/CrashDebug --elf build/aws_demos.elf --dump ../crash.hex.dmp
    0x0badc0de in ?? ()
    #0  0x0badc0de in ?? ()
    #1  0x0800f3ca in bhj_crash ()
    Backtrace stopped: Cannot access memory at address 0x1000258c
    A debugging session is active.

            Inferior 1 [Remote target] will be killed.

If not doing -ex "quit" gdb will stay open and can be used to inspect the dump.

With a crash one can now run `info mem` and show the memory regions. Those can then be used in `CrashCatcher_GetMemoryRegions()`.

