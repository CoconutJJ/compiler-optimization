# LLVM Stuff

Repository for working with LLVM passes. Codebase is configured to build and run
passes **out of tree** and also independent of **CMake**.

## LLVM Pass Compile and Link flags

The following command can be used to generate the library and linker flags needed
to compile a LLVM pass.

```sh
llvm-config --cxxflags --ldflags --libs --system-libs
```

I also had to explicitly specify the library and include paths for `libzstd` (which
I installed with `brew`)

The following command is used to 

```sh
```