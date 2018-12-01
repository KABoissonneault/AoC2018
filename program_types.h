#pragma once

#include <string>
#include <iosfwd>
#include <tl/expected.hpp>

struct input_error {
    std::string message;
    int line;
};

using output_data = tl::expected<std::string, input_error>;

using program_ptr = output_data(*)(std::istream& i);
