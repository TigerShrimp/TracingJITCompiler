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

- ./include/JVM/ByteCodes.hpp:262:    // TODO: WIDE is wierd, if operand after is iinc, then 5 operands, otherwise
- ./include/Definitions.hpp:9:// TODO: do this with std::format instead as soon as clang adds support for it.
- ./src/Interpreter.cpp:358:      // TODO: Make Long/Double constants more general
- ./src/RunTime.cpp:24:      // TODO: note is probably not a very good name.
- ./src/RunTime.cpp:29:        // TODO: Initiate trace recording
- ./src/RunTime.cpp:59:  // TODO: Standardize exception handing.

Created by Simon Kärrman och Jakob Erlandsson