# TigerShrimp

## Tracing JIT Compiler

This is the project for the TigerShrimp tracing JIT compiler,
an easy to understand tracing JIT compiler for the JVM language.

---

To build and run project from root:

- Clone project with "--recursive" flag, or initialize sub modules afterwards.

- Use cmake (version 3.19.0 or higher)

- make

- ./build/TigerShrimp

---

TODO:
./include/JVM/ByteCodes.hpp:260:    // TODO: WIDE is wierd, if operand after is iinc, then 5 operands, otherwise
./src/Interpreter.cpp:20:  // TODO: Standardize execption handing.
./src/Assembler.cpp:92:      // TODO: Find memory location by regex instead.

Created by Simon Kärrman och Jakob Erlandsson