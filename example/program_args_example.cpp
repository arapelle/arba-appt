#include <iostream>
#include <arba/appt/application/program_args.hpp>

int main(int argc, char** argv)
{
    appt::program_args args(argc, argv);
    for (const std::string_view& arg : args)
        std::cout << arg << std::endl;
    return EXIT_SUCCESS;
}
