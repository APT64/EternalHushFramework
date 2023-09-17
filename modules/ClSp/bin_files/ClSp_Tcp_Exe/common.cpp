#include <Windows.h>
#include <iostream>
#include <string>

std::string GLE()
{
    std::string err = std::to_string(GetLastError());

    return err;
}

