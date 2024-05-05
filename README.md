# Code Visualizer (C language)

Table of Contents:

- [Code Visualizer (C language)](#code-visualizer-c-language)
  - [About The Project](#about-the-project)
  - [Features](#features)
  - [Getting started](#getting-started)
  - [Usage](#usage)
  - [License](#license)
  - [Contact](#contact)

## About The Project

This is an implementation of the code visualization project for C language. This project represents a tool that consumes a codebase and generates an HTML version of it for better code reading.

Written in C++ using Clang Tooling libraries (part of LLVM).

## Features

- Syntax highlighting
- Scope collapsing
- Symbol referrences highlighting
- Pointing on symbols shows a declaration info
- Navigation within a codebase

## Getting started

This are instructions to build this tool on Windows:

- Install a C++ compiler with C++20 support (for example VS 2022)
- Install CMake (3.20+)
- Install a `vcpkg` package manager:

    ```PS
    git clone https://github.com/microsoft/vcpkg.git
    cd vcpkg && bootstrap-vcpkg.bat
    .\vcpkg.exe integrate install
    ```

- Install LLVM using `vcpkg`:

    ```PS
    vcpkg install llvm
    ```

    Since LLVM does not provide libraries to link against in Windows distributions we need to build it ourselves. This command will download LLVM sources and build distrbution package. This operation might take a couple of hours.
    > __Note__: I do not use manifest mode of `vcpkg` because LLVM distribution is too big to place inside a repository, so I prefer to use it globally

    After this operation you will receive a message from `vcpkg` with a flag with path to the CMake file that will hooks `vcpkg` packages into CMake projects. It will look something like this:

    ```PS
    -DCMAKE_TOOLCHAIN_FILE=<path/to/vcpkg>/scripts/buildsystems/vcpkg.cmake
    ```

    You need to copy this flag.
- Clone this repository

    ```PS
    git clone https://github.com/iu-code-visualization/code-visualizer-c.git
    ```

- Generate a project model using CMake. Provide CMake the path to `vcpkg` toolchain file.

    ```PS
    cmake -S . -B Build -DCMAKE_TOOLCHAIN_FILE=<path/to/vcpkg>/scripts/buildsystems/vcpkg.cmake
    ```

- Build the project using CMake or your favorite tool of choice.

    ```PS
    cmake --build Build
    ```

## Usage

The visualizer accepts a compilation database — a format of describing of source files in a C project. It behaves similarly as other Clang-based tool, such as `clang-format` or `clang-tidy`.

You can specify input files simply as arguments:

```PS
.\code-visualizer-c.exe main.c header.h ...
```

or use `compile_commands.json` if you build your project with CMake and use flag `-DCMAKE_EXPORT_COMPILE_COMMANDS=1`:

```PS
.\code-visualizer-c.exe -p compile_commands.json
```

## License

Distributed under the MIT License. See `LICENSE.txt` for more information.

## Contact

Anton Dospekhov - <tony.dospiekhov@gmail.com>
