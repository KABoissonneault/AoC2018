#include "program_types.h"

#include <vector>
#include <map>

#include <gsl/string_span>

#include <range/v3/utility/functional.hpp>
#include <range/v3/algorithm/count_if.hpp>
#include <range/v3/algorithm/find_if.hpp>

namespace
{
	using namespace std::string_literals;

	struct position { int x, y; };
	bool operator<(position lhs, position rhs) { return std::tie(lhs.x, lhs.y) < std::tie(rhs.x, rhs.y); }
	bool operator==(position lhs, position rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }
	struct distance { int w, h; };
	struct claim
	{
		int id;
		position starting_coord;
		distance size;
	};

	template<typename T>
	struct parse_state 
	{
		std::istream* i;
		int line_count;
		T* current_result;
	};

	using input_result = tl::expected<std::vector<claim>, input_error>;

	template<typename T>
	using parse_result = tl::expected<parse_state<T>, input_error>;

	struct parse_token {
		std::string token;
		auto operator()(parse_state<claim> state) -> parse_result<claim>
		{
			std::istream& i = *state.i;

			std::string next_token;
			if (!(i >> next_token))
			{
				return tl::make_unexpected<input_error>({ "Missing token after id", state.line_count });
			}

			if (next_token != token)
			{
				return tl::make_unexpected<input_error>({
					"Invalid token after id: expected '" + token + "', got '" + next_token + "'",
					state.line_count
					});
			}

			return state;
		}
	};

	auto parse_size(parse_state<claim> state) -> parse_result<claim>
	{
		std::istream& i = *state.i;

		if (!(i >> state.current_result->size.w))
		{
			return tl::make_unexpected<input_error>({ "Could not parse integer size w", state.line_count });
		}

		char c;
		if (!(i >> c) || c != 'x') {
			return tl::make_unexpected<input_error>({
				"Invalid token after w size: expected 'x', got '"s + c + "'",
				state.line_count
				});
		}

		if (!(i >> state.current_result->size.h))
		{
			return tl::make_unexpected<input_error>({ "Could not parse integer size h", state.line_count });
		}

		return state;
	}

	auto parse_coord(parse_state<claim> state) -> parse_result<claim>
	{
		std::istream& i = *state.i;

		if (!(i >> state.current_result->starting_coord.x))
		{
			return tl::make_unexpected<input_error>({ "Could not parse integer coordinate x", state.line_count });
		}

		char c;
		if (!(i >> c) || c != ',') {
			return tl::make_unexpected<input_error>({
				"Invalid token after x coordinate: expected ',', got '"s + c + "'",
				state.line_count
				});
		}

		if (!(i >> state.current_result->starting_coord.y))
		{
			return tl::make_unexpected<input_error>({ "Could not parse integer coordinate y", state.line_count });
		}

		return state;
	}

	auto parse_id(parse_state<claim> state) -> parse_result<claim> 
	{
		std::istream& i = *state.i;
		
		char c;
		if (!i.read(&c, 1) || c != '#') 
		{
			return tl::make_unexpected<input_error>({ "Expected '#' at the start of the id", state.line_count });
		}

		if (!(i >> state.current_result->id))
		{
			return tl::make_unexpected<input_error>({ "Could not parse integer id after '#'", state.line_count });
		}

		return state;
	}

	using claim_result = tl::expected<claim, input_error>;
	auto parse_claim(std::istream& i, int line_count) -> claim_result
	{
		claim c;
		parse_state<claim> state{ &i, line_count, &c };

		auto const parse_result = parse_id(std::move(state))
			.and_then(parse_token{ "@" })
			.and_then(parse_coord)
			.and_then(parse_token{ ":" })
			.and_then(parse_size);

		if (!parse_result)
		{
			return tl::make_unexpected(parse_result.error());
		}
		return c;
	}

	auto parse_input(std::istream& i) -> input_result
	{
		std::vector<claim> claims;
		int line_count = 1;
		while (i && i.peek() != EOF)
		{
			auto claim_result = parse_claim(i, line_count);
			if (!claim_result) 
			{
				return tl::make_unexpected(claim_result.error());
			}
			else 
			{
				claims.push_back(std::move(*claim_result));
			}
			++line_count;

			std::string endline;
			std::getline(i, endline);
		}
		return claims;
	}

	auto get_owned_positions(gsl::span<const claim> claims) 
		-> std::map<position, std::vector<int>> {
		std::map<position, std::vector<int>> owned_positions;
		for (claim const& c : claims) {
			int i = c.starting_coord.x;
			int const i_end = i + c.size.w;
			for (; i < i_end; ++i) {
				int j = c.starting_coord.y;
				int const j_end = j + c.size.h;
				for (; j < j_end; ++j) {
					owned_positions[position{ i, j }].push_back(c.id);
				}
			}
		}
		return owned_positions;
	}

}

auto program_day3_part1(std::istream& i)->output_data
{
	return parse_input(i).map([](std::vector<claim> const& claims) -> std::string {
		auto const owned_positions = get_owned_positions(claims);
		return std::to_string(ranges::count_if(owned_positions, [](auto const& position_value) { return position_value.second.size() > 1; }));
	});
}
auto program_day3_part2(std::istream& i) -> output_data
{
	return parse_input(i).map([](std::vector<claim> const& claims) -> std::string {
		auto const owned_positions = get_owned_positions(claims);
		std::map<int, bool> tainted_claims;
		for (auto const& [_, claims] : owned_positions)
		{
			if (claims.size() > 1) {
				for (auto const claim : claims) {
					tainted_claims[claim] = true;
				}
			} else {
				auto const claim = claims.front();
				if (tainted_claims.find(claim) == tainted_claims.end()) {
					tainted_claims[claim] = false;
				}
			}
		}

		auto const it_found = ranges::find_if(tainted_claims, [](auto const pair) { return !pair.second; });
		return std::to_string(it_found->first);
	});
}