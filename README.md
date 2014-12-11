## Overview

dasm-a64 is the ARM64 port of DynASM, the awesome dynamic assembler of 
[luajit](http://luajit.org/). All the instructions for AArch64 execution
state of ARMv8-A architecture profile are supported.

Comparing with the other ports of DynASM, A relative stupid encoding
schema is used: most encoding information is specified in the instruction
templates, the parse and encoding engine is relatively simple. Only very base
functions are provided in the encoding engine, the templates use these
base functions to specify the encoding for each instruction. So there may
be many different templates for one instructions(e.g. more than 100 templates
for ld1 instruction).

## Build

DynASM does not provide any library or executable, it would be used as the
preprocessor of any JIT compiler or dynamic assembler.

## Test

### Run Environment

I do not have access to a real arm64 device now, so dasm-a64 is tested on the
virtual machine provided by [Foundation Model](http://www.arm.com/fvp) from ARM.
The test environment is provided by [linaro](http://releases.linaro.org/latest/openembedded/aarch64/)

The lamp image is used for the simulation environment(there's arm64 gcc and gdb in this image):

    Foundation_v8pkg/models/Linux64_GCC-4.1/Foundation_v8 \
    --cores=4 \
    --no-secure-memory \
    --visualization \
    --gicv3 \
    --data=fvp_bl1.bin@0x0 \
    --data=fvp_fip.bin@0x8000000 \
    --network nat \
    --block-device=vexpress64-openembedded_lamp-armv8-gcc-4.9_20140923-688.img

### Test Result

A simple brainfxck virtual machine is provided in the test directory. This is
an ARM64 port from Peter Cawley's [tutorial of DynASM](http://corsix.github.io/dynasm-doc/tutorial.html)

  $ cd test/bf
  $ make test

The result looks like this:
    root@genericarmv8:~/dasm-a64/test/bf# make
    normal bf interpreter running...
        AAAAAAAAAAAAAAAABBBBBBB....
        ....
    real    22m 13.93s
    user    22m 13.69s
    sys     0m 0.00s
    
    jit running...
        AAAAAAAAAAAAAAAABBBBBB.....
        ....
    real    1m 51.86s
    user    1m 51.84s
    sys     0m 0.00s
    root@genericarmv8:~/dasm-a64/test/bf#

## Documentation

Peter Cawley provides an unoffical but great DynASM tutorial here:
[http://corsix.github.io/dynasm-doc/tutorial.html](http://corsix.github.io/dynasm-doc/tutorial.html)
