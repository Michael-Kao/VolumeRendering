#pragma once

#include <string>
#include <vector>

enum wordCase {
    NONE,
    LOWER,
    UPPER
};

namespace strUtil 
{
    std::vector<std::string> split(std::string const &target, std::string const &delimiter);
    std::string remove(std::string const &target, std::string const &delimiter, wordCase mode);
};