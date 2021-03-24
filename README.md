# TigerShrimp

## Tracing JIT Compiler

This is the project for the TigerShrimp tracing JIT compiler,
an easy to understand tracing JIT compiler for the JVM language.

---

Prerequisites:

- cmake (version 3.19.0)

- FMT (https://fmt.dev/latest/usage.html)

- JDK 15

- Python 3

To build and run project from root:

- Clone project with "--recursive" flag, or initialize sub modules afterwards.

- make

- ./build/TigerShrimp

---

TODO:

- ./include/x86.hpp:5:// TODO change name to Mnemonic
- ./include/JVM/ByteCodes.hpp:264:    // TODO: WIDE is wierd, if operand after is iinc, then 5 operands,
- ./include/Program.hpp:68:  // TODO: Remove these attributes
- ./include/Definitions.hpp:9:// TODO: do this with std::format instead as soon as clang adds support for it.
- ./src/Compiler.cpp:20:  // TODO make enter correct, needs offset to allign memory
- ./src/Compiler.cpp:40:    // TODO: Maybe change this to be a little more optimized
- ./src/Compiler.cpp:72:  // TODO: fill avail regs and xregs with all registers
- ./src/Interpreter.cpp:369:      // TODO: Make Long/Double constants more general
- ./src/RunTime.cpp:8:  // TODO: Either move this functionality to Compiler or remove
- ./src/RunTime.cpp:67:  // TODO: Standardize exception handing.

Created by Simon Kärrman och Jakob Erlandsson