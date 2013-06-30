liblon 0.1 - Lua Object Notation
================================

Disclaimer
----------

This project should be considered as a work in progress. At this stage, it is
far from robust, and should be treated as a learning exercise for writing
parsers in C, rather than anything suitable for use in a production
environment.

Please see the TODO file for a list of future plans for this project, and tasks
which could be accomplished by an interested contributor.

Overview
--------

The overall aim of this project is to create an object notation syntax based
upon the Lua programming language. Analogous to JSON for JavaScript, this would
be based upon a subset of Lua's syntax, allowing for the representation of
objects, but preventing unwanted execution of arbitrary code when interpreted.

Why
---

There's probably no compelling reason to use a Lua-based object syntax over
JSON or even XML, other than personal preference or interoperability with an
existing Lua codebase.

This project was originally created with the intention of using it to read
metadata for a software package system which already made considerable use of
Lua for its installation scripts.

Getting Started
---------------

The process of building this code on a UNIX-like system is quite straight-
forward. Clone the git repository, and `cd` into the working directory.

You'll need to make sure you have the CMake build system and a C compiler
installed on your system. The CMakeLists.txt file in this project uses
LLVM/Clang as its default compiler, but you can change this back to your
system's default C compiler by commenting out the relevant lines in the file.

To begin, run the `cmake` command to create Makefiles, then run the `make`
command to begin compiling the library and its associated tests.

You can now start programming against the library (see include/lon.h for the
API reference), and run the test application: `./tests/lex_test`.
