#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
#include <string>
#include <vector>

struct Set
{
	uint64_t blue = 0;
	uint64_t red = 0;
	uint64_t green = 0;

	bool isPossible(const Set& maxSet) const
	{
		return red <= maxSet.red
			&& green <= maxSet.green
			&& blue <= maxSet.blue;
	}

	uint64_t getPower() const { return red * green * blue; }
};

struct Game
{
	uint64_t id = 0;
	std::vector<Set> sets;

	Set getMinPossibleSet() const
	{
		Set minSet;

		for (const Set& set : sets)
		{
			minSet.red = std::max(minSet.red, set.red);
			minSet.green = std::max(minSet.green, set.green);
			minSet.blue = std::max(minSet.blue, set.blue);
		}

		return minSet;
	}
};

static std::vector<std::string> delimit(const std::string& str, const char delimiter)
{
	std::vector<std::string> tokens;

	size_t pos = 0;

	while (pos != std::string::npos)
	{
		const size_t nextPos = str.find(delimiter, pos);
		tokens.push_back(str.substr(pos, nextPos - pos));
		pos = nextPos != std::string::npos ? nextPos + 1 : std::string::npos;
	}

	return tokens;
}

static Game parseGame(const std::string& gameString)
{
	const std::vector<std::string> tokens = delimit(gameString, ' ');

	Game game;
	game.sets.push_back({});

	uint64_t currentNum = 0;

	for (const std::string& token : tokens)
	{
		Set& set = game.sets.back();

		if (token == "Game")
			assert(token == tokens.front());
		else if (game.id == 0)
			game.id = std::stoull(token);
		else if (std::isdigit(token.front()))
			currentNum = std::stoull(token);
		else if (token.find("red") != std::string::npos)
			set.red = currentNum;
		else if (token.find("green") != std::string::npos)
			set.green = currentNum;
		else if (token.find("blue") != std::string::npos)
			set.blue = currentNum;
		else
			assert(false);

		if (token.back() == ';')
			game.sets.push_back({});
	}

	return game;
}

static std::vector<Game> loadGames(const char* filename)
{
	std::vector<Game> games;

	std::fstream s{ filename, s.in };
	assert(s.is_open());

	while (s.peek() != EOF)
	{
		std::string line;
		std::getline(s, line);
		assert(line.size() > 0);

		games.push_back(parseGame(line));
		assert(games.back().id == games.size());
	}

	return games;
}

static uint64_t partOne(const std::vector<Game>& games)
{
	static constexpr Set maxSet = { 12, 13, 14 };
	static constexpr auto isPossible = [] (const Set& set) { return set.isPossible(maxSet); };

	return std::accumulate(begin(games), end(games), 0ull, [] (uint64_t sum, const Game& game)
	{
		return sum + (game.id * std::all_of(begin(game.sets), end(game.sets), isPossible));
	});
}

static uint64_t partTwo(const std::vector<Game>& games)
{
	return std::accumulate(begin(games), end(games), 0ull, [] (uint64_t sum, const Game& game)
	{
		return sum + game.getMinPossibleSet().getPower();
	});
}

static auto process(const char* filename)
{
	const std::vector<Game> games = loadGames(filename);
	const uint64_t partOneResult = partOne(games);
	const uint64_t partTwoResult = partTwo(games);
	return std::make_pair(partOneResult, partTwoResult);
}

void processPrintAndAssert(const char* filename, std::optional<std::pair<uint64_t, uint64_t>> expected = {})
{
	const auto result = process(filename);
	std::cout << "Part 1: " << result.first << " Part 2: " << result.second << std::endl;
	assert(!expected || result == expected);
}

void day02()
{
	processPrintAndAssert("../data/02/test.txt"), std::make_pair(8ull, 2286ull);
	processPrintAndAssert("../data/02/real.txt"), std::make_pair(2447ull, 56322ull);
}
