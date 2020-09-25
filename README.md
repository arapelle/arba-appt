# Concept

The purpose is to provide C++ application classes which embbeds useful tools (resource manager, event manager, virtual filesystem, ...).

See [task board](https://app.gitkraken.com/glo/board/X22qD23fUgARI-ki) for future updates and features.

# Install

## Requirements

Binaries:

- A C++20 compiler (ex: g++-10)
- CMake 3.16 or later

Libraries:

- [strn](https://github.com/arapelle/strn) 0.1.4
- [vlfs](https://github.com/arapelle/vlfs) 0.1.0
- [rsce](https://github.com/arapelle/rsce) 0.1.0
- [evnt](https://github.com/arapelle/evnt) 0.1.0
- [Google Test](https://github.com/google/googletest) 1.10 or later (only for testing)

## Clone

```
git clone https://github.com/arapelle/appt --recurse-submodules
```

## Quick Install

There is a cmake script at the root of the project which builds the library in *Release* mode and install it (default options are used).

```
cd /path/to/appt
cmake -P cmake_quick_install.cmake
```

Use the following to quickly install a different mode.

```
cmake -DCMAKE_BUILD_TYPE=Debug -P cmake_quick_install.cmake
```

## Uninstall

There is a uninstall cmake script created during installation. You can use it to uninstall properly this library.

```
cd /path/to/installed-appt/
cmake -P cmake_uninstall.cmake
```

# How to use

## Example - Use a simple *application*

```c++

```

## Example - Using *appt* in a CMake project

See the [basic cmake project](https://github.com/arapelle/appt/tree/master/example/basic_cmake_project) example, and more specifically the [CMakeLists.txt](https://github.com/arapelle/appt/tree/master/example/basic_cmake_project/CMakeLists.txt) to see how to use *appt* in your CMake projects.

# License

[MIT License](https://github.com/arapelle/appt/blob/master/LICENSE.md) © appt
