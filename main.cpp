#include <vector>
#include <string>
#include <cstdio>

#include <iostream>
#include <fstream>

#include <gsl/span>
#include <gsl/string_span>

#include <range/v3/algorithm/find.hpp>

#include "program_types.h"

namespace {
    auto get_puzzle_name(gsl::span<char const* const> args) -> std::string {
        if(args.size() < 2) {
            std::cout << "Enter the puzzle input identifer (DN): ";

            std::string filename;
            std::getline(std::cin, filename);

            std::cout << std::endl;
            
            return filename;
        } else {
            return args[1];
        }
    }
       
    struct puzzle {
        gsl::cstring_span<> name;
        program_ptr program;
    };

    auto test_program(std::istream& i) -> output_data {
        std::string line;
        std::string output;
        while(std::getline(i, line)) {
            output += line + "\n";
        }
        return output;
    }
}

auto program_day1_part1(std::istream& i)->output_data;
auto program_day1_part2(std::istream& i)->output_data;
auto program_day2_part1(std::istream& i)->output_data;
auto program_day2_part2(std::istream& i)->output_data;

struct puzzle puzzles[] = {
    {"test", &test_program},
    {"11", &program_day1_part1},
    {"12", &program_day1_part2},
    {"21", &program_day2_part1},
    {"22", &program_day2_part2},
};


int main(int argc, char const* argv[]) {
    std::string const puzzle_name = get_puzzle_name(gsl::make_span<char const* const>(argv, argc));

    auto const it_found = ranges::find(puzzles, puzzle_name, &puzzle::name);
    if(it_found == std::end(puzzles)) {
        std::cerr << "Invalid puzzle '" << puzzle_name << "'\n";
        return EXIT_FAILURE;
    }

    std::ifstream file(puzzle_name + ".txt");
    if(!file.is_open()) {
        std::cerr << "Input missing '" << puzzle_name << ".txt'\n";
        return EXIT_FAILURE;
    }

    output_data const output = it_found->program(file);
    if(!output) {
        input_error const& error = output.error();
        std::cerr << "Input error (line " << error.line << ") :" << error.message << "\n";
        return EXIT_FAILURE;
    }

    std::cout << *output;

    return EXIT_SUCCESS;
}