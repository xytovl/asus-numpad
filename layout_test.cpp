#include "layout.h"
#include <iostream>
#include <stdexcept>

int main(int argc, char **argv)
{
    try
    {
        if (argc == 2)
            parse_layout(argv[1]);
        else
            parse_layout(argv[1], argv[2]);
    }
    catch (const std::exception &e)
    {
        std::cerr << "error: " << e.what() << std::endl;
    }
}
