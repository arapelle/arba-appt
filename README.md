# Concept

The purpose is to provide C++ application classes which embbeds useful tools (resource manager, event manager, virtual filesystem, ...).

# Install

## Requirements

Binaries:
- A C++20 compiler (ex: g++-14)
- CMake 3.26 or later

Libraries:
- [spdlog](https://github.com/gabime/spdlog) 1.8  (required only if arba-appt::spdlogging is built)

Testing Libraries (optional):
- [Google Test](https://github.com/google/googletest) 1.14 or later  (optional)

## Clone

```
git clone https://github.com/arapelle/arba-appt
```

## Use with `conan`

- Create the conan package.
```
conan create . --build=missing -c
```
You can activate unit testing when creating the conan package with the following:
```
conan create . --build=missing -c:a "&:tools.build:skip_test=False"
```
- Add a requirement in your conanfile project file.
```python
    def requirements(self):
        self.requires("arba-appt/0.18.0")
```
If you want to disable a component, update your conan profile or use options with requires:
```python
    def requirements(self):
        self.requires("arba-appt/0.18.0", options={"spdlogging": False})
```

## Quick Install ##

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

## Example - An application with two modules

```c++
#include <arba/appt/application/basic_application.hpp>
#include <arba/appt/application/decorator/multi_task.hpp>
#include <arba/appt/application/module/basic_module.hpp>
#include <arba/appt/application/module/decorator/loop.hpp>

#include <iostream>
#include <random>

namespace example
{
class application : public appt::multi_task<appt::basic_application<>, application>
{
private:
    using base_ = appt::multi_task<appt::basic_application<>, application>;

public:
    using base_::base_;

    void init() { base_::init(); }
};

struct number_event
{
    unsigned number;
};

using module = appt::basic_module<application>;
template <class module_type>
using loop_module = appt::loop<module, module_type>;

class die12_module : public loop_module<die12_module>
{
private:
    using base_ = loop_module<die12_module>;

public:
    die12_module(application_type& app, std::string_view name = std::string_view())
        : base_(app, name), int_generator_(std::random_device{}())
    {
    }

    virtual ~die12_module() override = default;

    virtual void init() override
    {
        this->base_::init();
        numbers_.reserve(4);
    }

    void run_loop(appt::dt::seconds)
    {
        const unsigned number = die12();
        numbers_.push_back(number);

        std::ostringstream oss;
        oss << name() << ": [ ";
        for (unsigned n : numbers_)
            oss << n << " ";
        oss << " ]\n";
        std::cout << oss.str() << std::flush;

        if (numbers_.size() == numbers_.capacity())
            stop();
    }

    virtual void finish() override
    {
        std::cout << name() << " finished" << std::endl;
        app().stop_side_modules();
    }

private:
    unsigned die12()
    {
        static std::uniform_int_distribution<> die(1, 12);
        return die(int_generator_);
    }

private:
    std::mt19937_64 int_generator_;
    std::vector<unsigned> numbers_;
};

class die10_module : public loop_module<die10_module>
{
private:
    using base_ = loop_module<die10_module>;

public:
    die10_module(application_type& app, std::string_view name = std::string_view())
        : base_(app, name), int_generator_(std::random_device{}())
    {
    }

    virtual ~die10_module() override = default;

    void run_loop(appt::dt::seconds)
    {
        const unsigned number = die10();
        numbers_.push_back(number);

        std::ostringstream oss;
        oss << name() << ": [ ";
        for (unsigned n : numbers_)
            oss << n << " ";
        oss << " ]\n";
        std::cout << oss.str() << std::flush;
    }

    virtual void finish() override { std::cout << name() << " finished" << std::endl; }

private:
    unsigned die10()
    {
        static std::uniform_int_distribution<> die(1, 10);
        return die(int_generator_);
    }

private:
    std::mt19937_64 int_generator_;
    std::vector<unsigned> numbers_;
};

} // namespace example

int main(int argc, char** argv)
{
    example::application app(core::program_args(argc, argv));
    app.create_main_module<example::die12_module>("die12_module").set_frequency(2);
    app.create_module<example::die10_module>("die10_module").set_frequency(3);
    app.init();
    return app.run();
}
```

## Example - Using *arba-appt* in a CMake project

See *test_package* and more specifically the *CMakeLists.txt* to see how to use *arba-appt* in your CMake projects.

# License

[MIT License](./LICENSE.md) © arba-appt
