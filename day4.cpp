#define _CRT_SECURE_NO_WARNINGS

#include "program_types.h"

#include <variant>
#include <string>
#include <algorithm>

#include <gsl/string_span>

#include <range/v3/algorithm/sort.hpp>

namespace {
	struct command_base { std::string timestamp; };
	struct shift_open : command_base { int id; };
	struct sleep_start : command_base {};
	struct sleep_end : command_base {};

	using command = std::variant<shift_open, sleep_start, sleep_end>;
	auto get_command_timestamp(command const& c) -> gsl::cstring_span<> { return std::visit([](auto const& value) -> gsl::cstring_span<> { return value.timestamp; }, c); }
	auto get_command_date(command const& c) -> gsl::cstring_span<> {
		auto const time = get_command_timestamp(c);
		return time.subspan(0, 10); // YYYY-MM-DD
	}
	auto get_command_minute(command const& c) -> gsl::cstring_span<> {
		auto const time = get_command_timestamp(c);
		return time.subspan(time.size() - 2, 2);
	}
	bool command_less(command const& lhs, command const& rhs) { return get_command_timestamp(lhs) < get_command_timestamp(rhs); }


	using input_result = tl::expected<std::vector<command>, input_error>;
	auto parse_input(std::istream& i) -> input_result {
		std::vector<command> commands;

		std::string line;
		int line_count = 0;
		while (std::getline(i, line)) {
			auto const timestamp_start = line.data() + line.find('[') + 1;
			auto const timestamp_end = line.data() + line.find(']');
			auto timestamp = std::string(timestamp_start, timestamp_end);

			auto const command_start = timestamp_end + 2;
			auto const command_end = line.data() + line.size();

			std::string const command_description(command_start, command_end);
			if (command_description == "falls asleep") {
				commands.emplace_back(sleep_start{ std::move(timestamp) });
			}
			else if (command_description == "wakes up") {
				commands.emplace_back(sleep_end{ std::move(timestamp) });
			}
			else {
				int id;
				if (sscanf(command_description.c_str(), "Guard #%d begins shift", &id) != 1) {
					return tl::make_unexpected(input_error{ "Could not parse guard id in shift command", line_count });
				}
				commands.emplace_back(shift_open{ std::move(timestamp), id });
			}
			line_count++;
		}

		return commands;
	}

	struct schedule
	{
		int shift_id;
	};
}

auto program_day4_part1(std::istream& i) -> output_data {
	return parse_input(i).map([](std::vector<command> commands) -> std::string {
		ranges::sort(commands, command_less);
	});
}

auto program_day4_part2(std::istream& i) -> output_data {
	return "unimplemented";
}