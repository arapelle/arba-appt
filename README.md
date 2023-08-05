# Concept

The purpose is to provide C++ application classes which embbeds useful tools (resource manager, event manager, virtual filesystem, ...).

# Install

## Requirements

Binaries:
- A C++20 compiler (ex: g++-13)
- CMake 3.26 or later

Libraries:
- [arba-rsce](https://github.com/arapelle/arba-rsce) 0.2.0
- [arba-evnt](https://github.com/arapelle/arba-evnt) 0.3.0
- [spdlog](https://github.com/gabime/spdlog) 1.8

Testing Libraries (optional):
- [Google Test](https://github.com/google/googletest) 1.13 or later  (optional)

## Clone

```
git clone https://github.com/arapelle/arba-appt --recurse-submodules
```

## Quick Install

There is a cmake script at the root of the project which builds the library in *Release* mode and install it (default options are used).

```
cd /path/to/arba-appt
cmake -P cmake/scripts/quick_install.cmake
```

Use the following to quickly install a different mode.

```
cmake -P cmake/scripts/quick_install.cmake -- TESTS BUILD Debug DIR /tmp/local
```

## Uninstall

There is a uninstall cmake script created during installation. You can use it to uninstall properly this library.

```
cd /path/to/installed-arba-appt/
cmake -P uninstall.cmake
```

# How to use

## Example - Print program arguments

```c++
#include <iostream>
#include <arba/appt/application/program_args.hpp>

int main(int argc, char** argv)
{
    appt::program_args args(argc, argv);
    for (const std::string_view& arg : args)
        std::cout << arg << std::endl;
    return EXIT_SUCCESS;
}
```

## Example - Using *arba-appt* in a CMake project

See *basic_cmake_project* in example, and more specifically the *CMakeLists.txt* to see how to use *arba-appt* in your CMake projects.

# License

[MIT License](./LICENSE.md) © arba-appt
