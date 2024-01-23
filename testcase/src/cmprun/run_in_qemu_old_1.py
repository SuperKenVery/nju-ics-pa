import argparse
import subprocess
import tempfile
import os

parser = argparse.ArgumentParser(description="Convert a binary image to a kernel.elf file")
parser.add_argument("binary_filename", help="Name of the binary image")
args = parser.parse_args()

imgname=args.binary_filename
imgname=os.path.join(
    os.getcwd(),
    imgname
)
img_basename='.'.join(imgname.split('.')[:-1])

bootloader=f'''
bits 32
global _start

MB1_MAGIC    equ 0x1badb002
MB1_FLAGS    equ 0x00000000
MB1_CHECKSUM equ -(MB1_MAGIC+MB1_FLAGS)

section .data
align 4
    dd MB1_MAGIC
    dd MB1_FLAGS
    dd MB1_CHECKSUM

section .text
_start:
    incbin "{imgname}"  
'''

with tempfile.NamedTemporaryFile('w') as bootloader_asm_file,\
    tempfile.NamedTemporaryFile('w') as bootloader_bin_file:
    bootloader_asm_file.write(bootloader)
    print("Compiling bootloader...")
    subprocess.run(['nasm','-felf32',bootloader_asm_file.name,'-o',bootloader_bin_file.name])
    print("Linking bootloader with img...")
    subprocess.run(['ld','-Ttext=0x30000','-melf_i386',bootloader_bin_file.name,'-o',img_basename+'_qemu.elf'])

print("Done.")

