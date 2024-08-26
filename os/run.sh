#!/bin/bash
set -xue

QEMU=qemu-system-riscv32
if [ "$(uname)" == 'Darwin' ]; then
  CC=/opt/homebrew/opt/llvm/bin/clang
  OBJCOPY=/opt/homebrew/opt/llvm/bin/llvm-objcopy
elif [ "$(expr substr $(uname -s) 1 5)" == 'Linux' ]; then
  CC=clang
  OBJCOPY=/usr/bin/llvm-objcopy
else
  echo "Your platform ($(uname -a)) is not supported."
  exit 1
fi
CFLAGS="-std=c11 -O2 -g3 -Wall -Wextra --target=riscv32 -ffreestanding -nostdlib"

# シェルをビルド
$CC $CFLAGS -Wl,-Tuser.ld -Wl,-Map=shell.map -o shell.elf shell.c user.c common.c
$OBJCOPY --set-section-flags .bss=alloc,contents -O binary shell.elf shell.bin
$OBJCOPY -Ibinary -Oelf32-littleriscv shell.bin shell.bin.o

# カーネルをビルド
$CC $CFLAGS -Wl,-Tkernel.ld -Wl,-Map=kernel.map -o kernel.elf \
  kernel.c common.c shell.bin.o

# QEMUを起動
$QEMU -machine virt -bios default -nographic -serial mon:stdio --no-reboot \
  -kernel kernel.elf
