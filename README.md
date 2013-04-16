# QEMU with STM32F2xx Microcontroller Implementation

## Overview
This is a copy of QEMU that has been modified to include an implementation of the STM32F2xx microcontroller.
This is based off of a QEMU fork that is targeting the STM32F103: https://github.com/beckus/qemu_stm32
This repo contains both beckus' STM32F1xx implementation and my STM32F2xx additions.
My repo is a bit more up to date (QEMU v1.4) than beckus' repo.

__DANGER DANGER: It is very much a work-in-progress! Only some of the peripherals are working at the moment. Please contribute!__

### Status
- Working alias for the program flash memory region (see "Linking" below).
- SYSCFG mostly done.
- RCC partially done, clock tree set up for the larger part, many control/config registers stubbed with "unimplemented warnings".

## Building

Configure options that I have found useful when developing (tested only on OS X 10.8.3 / Mountain Lion).

        ./configure --enable-tcg-interpreter --extra-ldflags=-g \
        --with-coroutine=gthread --enable-debug-tcg --enable-cocoa \
        --enable-debug --disable-werror --target-list="arm-softmmu" \
        --extra-cflags=-DDEBUG_CLKTREE --extra-cflags=-DDEBUG_STM32_RCC \
        --extra-cflags=-DDEBUG_STM32_UART --extra-cflags=-DSTM32_UART_ENABLE_OVERRUN \
        --extra-cflags=-DDEBUG_GIC

Some comments on the flags:

- The `--enable-tcg-interpreter` enables the "JIT" module (called Tiny Code Generator) and is already on by default. I like keeping the flag around to make it easy to turn off (and use the byte code interpreter instead).
- The `DEBUG_...` defines were already there in beckus' implementations of the STM peripherals and basically turn on logging on important events for these peripherals.
- Flags I should look at whether they are actually needed: `--enable-cocoa` (likely to be automatically enabled on OS X), `--extra-ldflags=-g` not sure what this does.

Minimal configuration for a typical build:

        ./configure --disable-werror --enable-debug --target-list="arm-softmmu"
        make

Useful make commands when rebuilding:

        make defconfig
        make clean

## Running
The generated executable is arm-softmmu/qemu-system-arm .

Example:

        qemu-system-arm -M stm32-p205 -s -S -kernel your_firmware.elf

- `-M stm32-p205` specifies that the machine with identifier `stm32-p205` should be used. This is the ID of the fictive board/machine that I'm using to test stuff out. See `stm32p205.c`.

- `-s` Is a shorthand for -gdb tcp::1234, i.e. open a gdbserver on TCP port 1234.

- `-S` Will halt the CPU at startup, (you must type 'c' in the monitor or in GDB).

- `-kernel your_firmware.elf` The firmware you want to load.

There is some weirdness with signals and GDB / LLDB under OS X, it mucks around with them. `sigaction()` sometimes returns an error and doesn't set `action`. See `sigfd_handler()` in `main-loop.c`.

I also recommend adding the following to your `.lldbinit`:

		process handle -p true -s false SIGUSR1
		process handle -p true -s false SIGUSR2

This will make LLDB not break on SIGUSR1 and SIGUSR2, which QEMU makes use of for its interrupt emulation.

## Linking

QEMU's ARM core expects the firmware to be loaded into a region starting at address `0x00000000`. STM32's flash program space is at `0x08000000` and what appears at `0x00000000` depends on the configuration of the `BOOT` pins.

Long story short, I wanted to use the same linker script that I'm using for the real hardware, so I added an address translation function and an alias region that makes a reference at `0x08000000 - ...` to `0x00000000 - ...`. See `kernel_load_translate_fn()` and the `flash_alias` region in the init function in `stm32f2xx.c`.

## Using arm-non-eabi-gdb

After you've started `qemu-system-arm` with `-s` (and optionally `-S`), you can attach to the virtual STM32 as follows:

        $ arm-non-eabi-gdb your_firmware.elf
        (gdb) target remote :1234
		Remote debugging using :1234
		0x08000a00 in Reset_Handler ()
		(gdb) 

All the QEMU monitor commands are also available via GDB by prefixing it with `monitor` like so:

		(gdb) monitor info mtree
		memory
		0000000000000000-7ffffffffffffffe (prio 0, RW): system
		  0000000000000000-00000000000fffff (prio 0, R-): armv7m.flash
		...
		
		(gdb) monitor system_reset
		...
		
## QEMU Monitor

There are a lot of useful commands that you can punch into the QEMU monitor.
Here's a list: http://qemu.weilnetz.de/qemu-doc.html#pcsys_005fmonitor

## QEMU Docs
Read original the documentation in qemu-doc.html or on http://wiki.qemu.org

## License

The following points clarify the QEMU license:

1. QEMU as a whole is released under the GNU General Public License

2. Parts of QEMU have specific licenses which are compatible with the
GNU General Public License. Hence each source file contains its own
licensing information.

Many hardware device emulation sources are released under the BSD license.

3. The Tiny Code Generator (TCG) is released under the BSD license
   (see license headers in files).

4. QEMU is a trademark of Fabrice Bellard.
