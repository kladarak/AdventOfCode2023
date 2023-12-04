#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
#include <ranges>
#include <set>
#include <string>
#include <vector>

struct Game04
{
	uint64_t id = 0;
	uint64_t numMatching = 0;
	uint64_t numCopies = 1;

	std::vector<uint64_t> winningNums;
	std::vector<uint64_t> cardNums;

	void evaluateMatches()
	{
		std::vector<uint64_t> intersection;

		std::set_intersection(
			begin(winningNums), end(winningNums),
			begin(cardNums), end(cardNums),
			std::back_inserter(intersection));

		numMatching = intersection.size();
	}

	uint64_t score() const
	{
		uint64_t score = 0;

		for (uint64_t i = 0; i < numMatching; ++i)
		{
			if (score)
				score *= 2;
			else
				score = 1;
		}

		return score;
	}
};

static Game04 parseGame(const std::string& gameString)
{
	Game04 game;

	bool parsingWinning = true;

	for (const auto word : std::views::split(gameString, ' '))
	{
		const std::string token(&*word.begin(), std::ranges::distance(word));

		if (token.empty())
			continue;
		else if (token == "Card")
			continue;
		else if (game.id == 0)
			game.id = std::stoull(token);
		else if (std::isdigit(token.front()))
			(parsingWinning ? game.winningNums : game.cardNums).push_back(std::stoull(token));
		else if (token == "|")
			parsingWinning = false;
		else
			assert(false);
	}

	std::sort(begin(game.winningNums), end(game.winningNums));
	std::sort(begin(game.cardNums), end(game.cardNums));

	game.evaluateMatches();

	return game;
}

static void evaluateCopies(auto& games)
{
	for (uint64_t i = 0; i < games.size(); ++i)
	{
		const Game04& current = games[i];

		for (uint64_t j = 0; j < current.numMatching; ++j)
			games[i + j + 1].numCopies += current.numCopies;
	}
}

static auto loadData(const char* filename)
{
	std::fstream s{ filename, s.in };
	assert(s.is_open());

	std::vector<Game04> games;

	while (s.peek() != EOF)
	{
		std::string line;
		std::getline(s, line);
		assert(line.size() > 0);
		games.push_back(parseGame(line));
	}

	evaluateCopies(games);

	return games;
}

static uint64_t partOne(const auto& data)
{
	return std::accumulate(
		begin(data),
		end(data),
		0ull,
		[] (uint64_t sum, const auto& game) { return sum + game.score(); }
	);
}

static uint64_t partTwo(const auto& data)
{
	return std::accumulate(
		begin(data),
		end(data),
		0ull,
		[] (uint64_t sum, const auto& game) { return sum + game.numCopies; }
	);
}

static std::pair<uint64_t, uint64_t> process(const char* filename)
{
	const auto data = loadData(filename);
	return std::make_pair(partOne(data), partTwo(data));
}

static void processPrintAndAssert(const char* filename, std::optional<std::pair<uint64_t, uint64_t>> expected = {})
{
	const auto result = process(filename);
	std::cout << "Part 1: " << result.first << " Part 2: " << result.second << std::endl;

	assert(!expected || expected.value().first == 0 || result.first == expected.value().first);
	assert(!expected || expected.value().second == 0 || result.second == expected.value().second);
}

void day04()
{
	processPrintAndAssert("../data/04/test.txt", std::make_pair(13ull, 30ull));
	processPrintAndAssert("../data/04/real.txt", std::make_pair(23750ull, 13261850ull));
}
