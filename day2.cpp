#include "program_types.h"

#include <istream>
#include <vector>
#include <utility>
#include <map>

#include <tl/expected.hpp>
#include <range/v3/numeric/accumulate.hpp>

namespace {
    using input_result = tl::expected<std::vector<std::string>, input_error>;

    auto parse_input(std::istream& i) -> input_result {
        std::vector<std::string> result;
        std::string line;
        while(std::getline(i, line)) {
            result.push_back(std::move(line));
        }

        return result;
    }
}

auto program_day2_part1(std::istream& i) -> output_data {
    return parse_input(i).map([] (std::vector<std::string> const& values) {
        auto const counts = ranges::accumulate(values, std::make_pair(0, 0), [] (auto const counts, std::string const& v) -> std::pair<int, int> {
            std::map<char, int> chars;
            for(char c : v) {
                ++chars[c];
            }
            bool has_two = false;
            bool has_three = false;

            for(auto const& char_pair : chars) {
                if(char_pair.second == 2) {
                    has_two = true;
                } else if(char_pair.second == 3) {
                    has_three = true;
                }
            }

            return {counts.first + (has_two ? 1 : 0), counts.second + (has_three ? 1 : 0)};
        });
        return std::to_string(counts.first * counts.second);
    });
}

auto program_day2_part2(std::istream& i) -> output_data {
    return parse_input(i).and_then([] (std::vector<std::string> const& values) -> output_data {
        for(int i = 0; i < values.size(); ++i) {
            std::string const& value = values[i];
            for(int j = i; j < values.size(); ++j) {
                std::string const& test = values[j];
                int diff_count = 0;
                int last_diff;
                for(int k = 0; k < value.size(); ++k) {
                    if(value[k] != test[k]) {
                        ++diff_count;
                        last_diff = k;
                        if(diff_count == 2) {
                            break;
                        }
                    }
                }
                if(diff_count == 1) {
                    std::string result = value;
                    result.erase(result.begin() + last_diff);
                    return result;
                }
            }
        }
        return tl::make_unexpected(input_error{"Puzzle logic invalid"});
    });
}