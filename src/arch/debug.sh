#!/bin/bash
set -euo pipefail

qemu-system-x86_64 \
    -cdrom foxos.iso \
    -m 512M \
    -vga std \
    -s -S &

gdb-multiarch foxos.bin \
    -ex "set architecture i386:x86_64" \
    -ex "target remote :1234" \
    -ex "break kernel_main"