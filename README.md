# TigerShrimp

## Tracing JIT Compiler

This is the project for the TigerShrimp tracing JIT compiler,
an easy to understand tracing JIT compiler for the JVM language.

---

Prerequisites:

- cmake (version 3.19.0)

- JDK 15

- Python 3

To build and run project from root:

- Clone project with "--recursive" flag, or initialize sub modules afterwards.

- make

- ./build/TigerShrimp

---

TODO:

- ./include/JVM/ByteCodes.hpp:262:    // TODO: WIDE is wierd, if operand after is iinc, then 5 operands, otherwise
- ./src/Interpreter.cpp:113:  // TODO: Standardize execption handing.
- ./src/Interpreter.cpp:200:        // TODO: Make Long/Double constants more general
- ./src/Assembler.cpp:92:      // TODO: Find memory location by regex instead.

Created by Simon Kärrman och Jakob Erlandsson