#include "program_types.h"

#include <istream>
#include <vector>

#include <tl/expected.hpp>

#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/algorithm/find.hpp>
#include <range/v3/view/cycle.hpp>

namespace {
    using input_result = tl::expected<std::vector<int>, input_error>;

    auto parse_input(std::istream& i) -> input_result {
        std::vector<int> v;
        std::string line;
        int linecount = 0;
        while(std::getline(i, line)) {
            if(line.size() <= 1) {
                return tl::make_unexpected(input_error{"Insufficient characters in the line", linecount});
            }

            char* str_end;
            int const data = std::strtol(line.c_str(), &str_end, 10);
            if(str_end == line.c_str()) {
                return tl::make_unexpected<input_error>({"Invalid instruction on line: " + line, linecount});
            }
            
            v.push_back(data);

            ++linecount;
        }

        return v;
    }
}

auto program_day1_part1(std::istream& i) -> output_data {
    return parse_input(i).map([] (std::vector<int> const& input) -> std::string {
        return std::to_string(ranges::accumulate(input, 0));
    });
}

auto program_day1_part2(std::istream& i) -> output_data {
    return parse_input(i).map([] (std::vector<int> const& input) -> std::string { 
        std::vector<int> frequencies{0};
        auto const v = ranges::view::cycle(input);

        size_t i = 0;
        while(true) {
            int const new_frequency = frequencies[i] + v[i];

            if(ranges::find(frequencies, new_frequency) != frequencies.end()) {
                return std::to_string(new_frequency);
            }

            frequencies.push_back(new_frequency);

            ++i;
        }
    });
}