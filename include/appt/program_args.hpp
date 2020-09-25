#pragma once

#include <filesystem>
#include <iterator>

inline namespace arba
{
namespace appt
{
class program_args
{
private:
    class arg_iterator
    {
    public:
        using difference_type = std::iterator_traits<char**>::difference_type;
        using value_type = std::iterator_traits<char**>::value_type;
        using pointer = std::iterator_traits<char**>::pointer;
        using reference = std::string_view;
        using iterator_category = std::iterator_traits<char**>::iterator_category;

        explicit arg_iterator(char** arg_ptr = nullptr)
        : arg_ptr_(arg_ptr)
        {}

        inline reference operator*() const { return *arg_ptr_; }
        inline reference operator*() { return *arg_ptr_; }
        inline arg_iterator& operator++() { ++arg_ptr_; return *this; }
        inline arg_iterator operator++(int) { arg_iterator res(*this); ++(*this); return res; }
        inline arg_iterator& operator--() { --arg_ptr_; return *this; }
        inline arg_iterator operator--(int) { arg_iterator res(*this); --(*this); return res; }
        inline arg_iterator operator+(std::ptrdiff_t off) const { return arg_iterator(arg_ptr_ + off); }
        inline arg_iterator operator-(std::ptrdiff_t off) const { return arg_iterator(arg_ptr_ - off); }
        inline bool operator==(const arg_iterator& rhs) const = default;
        inline bool operator!=(const arg_iterator& rhs) const = default;

    private:
        char** arg_ptr_;
    };

public:
    using iterator = arg_iterator;
    using const_iterator = const arg_iterator;

    program_args(int ac, char** av)
        : argc(ac), argv(av)
    {}

    std::size_t size() const { return argc; }
    inline const_iterator begin() const { return const_iterator(argv); }
    inline const_iterator end() const { return const_iterator(argv + argc); }
    inline iterator begin() { return iterator(argv); }
    inline iterator end() { return iterator(argv + argc); }
    inline const_iterator cbegin() const { return begin(); }
    inline const_iterator cend() const { return end(); }

    inline std::string_view operator[](std::size_t index) const { return argv[index]; }
    inline std::string_view operator[](std::size_t index) { return argv[index]; }

    inline std::filesystem::path program_path() const { return argv[0]; }
    inline std::filesystem::path program_dir() const { return program_path().parent_path(); }
    inline std::filesystem::path program_name() const { return program_path().filename(); }
    inline std::filesystem::path program_stem() const { return program_path().stem(); }

    const std::size_t argc;
    char**const argv;
};
}
}
