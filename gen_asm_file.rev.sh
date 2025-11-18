#!/bin/bash

#export PYTHONPATH=../gitpfn/dlchip-aics/assembler
export PYTHONPATH=/home/ext-ryos36/codegen/070b786d8/pfcomp/gpfn2

MAIN=/home/ext-ryos36/codegen/070b786d8/pfcomp/gpfn2/assembler/gpfnasm/main.py
PACKER_MAIN=/home/ext-ryos36/codegen/070b786d8/pfcomp/codegen/build/grape_pfn/packer/packer_main
IMPACKER=/home/ext-ryos36/codegen/070b786d8/pfcomp/gpfn2/driver/packer/bin/impacker

#../gitpfn/dlchip-aics/assembler/gpfnasm/main.py --mino --print-asm  --print-comment $1 | ../gitpfn/dlchip-aics/emulator/grape_pfn/packer/packer_main -i /dev/stdin | ../gitpfn/dlchip-aics/driver/packer/bin/impacker /dev/stdin > $1.asm
$MAIN --mino --print-asm  --print-comment $1 | $PACKER_MAIN -i /dev/stdin | $IMPACKER /dev/stdin > $1.asm

