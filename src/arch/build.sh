#!/usr/bin/bash
set -euo pipefail

# Check dependencies
check_dependency() {
    if ! command -v "$1" &> /dev/null; then
        echo "Error: Required tool '$1' not found."
        echo "Install command: sudo apt install -y $2"
        exit 1
    fi
}

echo "Checking build dependencies..."
check_dependency "nasm" "nasm"
check_dependency "g++" "g++"
check_dependency "ld" "binutils"
check_dependency "grub-mkrescue" "grub2-common grub-pc-bin"
check_dependency "xorriso" "xorriso"
echo "Dependencies check passed."

# Clean old files
echo "Cleaning old build files..."
rm -rf iso foxos.bin boot.o kernel.o foxos.iso
echo "Clean completed."

# Assemble boot.s
echo "Assembling boot.s..."
nasm -f elf32 boot.s -o boot.o
if [ ! -f "boot.o" ]; then
    echo "Error: boot.s assembly failed."
    exit 1
fi
echo "boot.s assembled successfully."

# Compile kernel.cpp
echo "Compiling kernel.cpp..."
g++ -ffreestanding -m32 -c ../kernel/kernel.cpp -o kernel.o -Wall -Wextra -Werror
if [ ! -f "kernel.o" ]; then
    echo "Error: kernel.cpp compilation failed."
    exit 1
fi
echo "kernel.cpp compiled successfully."

# Link kernel binary
echo "Linking kernel to foxos.bin..."
ld -m elf_i386 -T linker.ld boot.o kernel.o -o foxos.bin
if [ ! -f "foxos.bin" ]; then
    echo "Error: Kernel linking failed."
    exit 1
fi
echo "foxos.bin generated successfully."

# Prepare ISO structure
echo "Preparing ISO directory structure..."
mkdir -p iso/boot/grub
cp foxos.bin iso/boot/
cp grub.cfg iso/boot/grub/

if [ ! -f "iso/boot/grub/grub.cfg" ]; then
    echo "Error: grub.cfg not found in ISO directory."
    exit 1
fi

# Generate bootable ISO
echo "Creating foxos.iso..."
grub-mkrescue -o foxos.iso iso/ --verbose
if [ ! -f "foxos.iso" ]; then
    echo "Error: ISO creation failed."
    exit 1
fi

echo "Build completed successfully."
echo "Run command: qemu-system-x86_64 -cdrom foxos.iso -m 512M -vga std -serial stdio -boot d"