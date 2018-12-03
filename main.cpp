#include <vector>
#include <set>
#include <string>
#include <cstdio>

#include <iostream>
#include <fstream>

#include <gsl/span>
#include <gsl/string_span>

#include <range/v3/algorithm/find.hpp>

#include "program_types.h"

namespace {
	struct puzzle_identifier
	{
		std::string day;
		std::string part;
	};

    auto get_puzzle_name(gsl::span<char const* const> args) -> puzzle_identifier {
        if(args.size() < 3) {
            std::cout << "Enter the puzzle input identifer (D N): ";

			puzzle_identifier identifier;
			std::cin >> identifier.day;
			std::cin >> identifier.part;

            std::cout << std::endl;
            
            return identifier;
        } else {
			return { args[1], args[2] };
        }
    }
       
	struct puzzle_part
	{
		gsl::cstring_span<> name;
		program_ptr program;
	};
	bool operator<(puzzle_part const& lhs, puzzle_part const& rhs)
	{
		return lhs.name < rhs.name;
	}
	bool operator==(puzzle_part const& lhs, puzzle_part const& rhs)
	{
		return lhs.name == rhs.name;
	}

    struct puzzle 
	{
        gsl::cstring_span<> name;
		std::set<puzzle_part> parts;
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
	{"1", { {"1", &program_day1_part1}, {"2", &program_day1_part2} } },
	{"2", { {"1", &program_day2_part1}, {"2", &program_day2_part2} } },
};


int main(int argc, char const* argv[]) {
    puzzle_identifier const puzzle_name = get_puzzle_name(gsl::make_span<char const* const>(argv, argc));

    auto const it_found_day = ranges::find(puzzles, puzzle_name.day, &puzzle::name);
    if(it_found_day == std::end(puzzles)) {
        std::cerr << "Invalid puzzle day '" << puzzle_name.day << "'\n";
        return EXIT_FAILURE;
    }

    std::ifstream file(puzzle_name.day + ".txt");
    if(!file.is_open()) {
        std::cerr << "Input missing '" << puzzle_name.day << ".txt'\n";
        return EXIT_FAILURE;
    }

	puzzle const& day = *it_found_day;
	auto const it_found_part = ranges::find(day.parts, puzzle_name.part, &puzzle_part::name);
	if (it_found_part == std::end(day.parts)) {
		std::cerr << "Invalid puzzle part '" << puzzle_name.part << "'\n";
		return EXIT_FAILURE;
	}

    output_data const output = it_found_part->program(file);
    if(!output) {
        input_error const& error = output.error();
        std::cerr << "Input error (line " << error.line << ") :" << error.message << "\n";
        return EXIT_FAILURE;
    }

    std::cout << *output;

    return EXIT_SUCCESS;
}