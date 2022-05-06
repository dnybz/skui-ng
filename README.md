# Introduction

Skia-based C++ UI framework Next Generation fork from https://github.com/skui-org/skui

UI framework that uses [Skia](https://skia.org/) as a low-level drawing toolkit.
It uses the newest features of the C++ Standard library (currently targetting C++17).

Thank Ruben Van Boxem!

# Status

SkUI-Ng is in a pre-release development phase, and the API is definitely not stable.

| Operating System | Compiler                | Supported          | Notes |
|------------------| ------------------------|--------------------| ----- |
| Windows          | Visual Studio 2019      | :heavy_check_mark: |       |
| Windows          | VS 2019 Clang/LLVM 8.0+ | :heavy_check_mark: |       |
| Windows          | Visual Studio 2017      | :heavy_check_mark: |       |
| Windows          | VS 2017 Clang/LLVM 6.0+ | :heavy_check_mark: |       |
| Windows          | MinGW-w64 GCC 7.3+      | :heavy_check_mark: | Requires usable std::filesystem implementation. |
| Windows          | MinGW-w64 Clang 6.0+    | :heavy_check_mark: | Requires usable std::filesystem implementation. |
| Windows          | Intel C++ 19            |        :x:         | Lacks required C++17 features. |
| Linux            | GCC 7.3+                | :heavy_check_mark: |       |
| Linux            | Clang 6.0+              | :heavy_check_mark: |       |
| Linux            | Intel C++ 19            |        :x:         | Lacks required C++17 features. |
| Mac OS X 10.15+  | XCode 11.6+ Clang       |      :soon:        | Missing implementation of core window/application classes. |
| Mac OS X 10.15+  | Homebrew GCC 7.3+       |      :soon:        | Missing implementation of core window/application classes. |
| Mac OS X 10.15+  | Homebrew Clang 6.0+     |      :soon:        | Missing implementation of core window/application classes. |

# Components

SkUI-Ng is subdivided in several modules which can depend on other modules but shouldn't become a dependency mess:

 * Core: basic functionality, including signals, properties, strings, paths, application, os abstraction...
 * CSS: CSS parser built on top of Boost.Spirit X3.
 * Graphics: graphical functionality such as canvases, contexts, shapes, text, ...
 * GUI: abstraction of platform-specific UI code, including event loops, windows, input, ...
 * OpenGL: abstraction of platform-specific OpenGL initialization code, header differences, ...
 * ...
 * Examples: example programs showing SkUI-Ng features
 * Tests: unit tests for various components to ensure correct and expected behaviour now and in the future.

This list will grow (and change) as the library's design takes shape.

# How to get started

Clone this repository

    git clone https://github.com/dnybz/skui-ng.git

After cloning this repository, make sure the submodules are up to date

    cd skui-ng
    git submodule update --init --recursive

Create and navigate to a seperate build directory, e.g.

    mkdir ../skui-build
    cd ../skui-build

Run CMake (here, the [ninja](https://ninja-build.org/) build system is used, but any generator should work fine)

    cmake ../skui -G Ninja

This step should inform you of any missing dependencies.
Then build

    cmake --build .

And run the tests to ensure nothing is wrong

    ctest