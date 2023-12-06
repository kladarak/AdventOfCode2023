#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
#include <ranges>
#include <string>
#include <vector>
#include <unordered_map>

struct Race
{
	uint64_t duration = 0;
	uint64_t recordDistance = 0;

	uint64_t calculateNumWaysOfWinning() const
	{
		uint64_t winCount = 0;

		for (uint64_t i = 0; i < duration; ++i)
		{
			const uint64_t speed = i;
			const uint64_t timeRemaining = duration - i;
			const uint64_t distance = speed * timeRemaining;

			winCount += distance > recordDistance;
		}

		return winCount;
	}
};

static auto loadDataPartOne(const char* filename)
{
	std::fstream s{ filename, s.in };
	assert(s.is_open());

	std::vector<Race> races;

	while (s.peek() != EOF)
	{
		std::string line;
		std::getline(s, line);
		assert(line.size() > 0);

		const bool parseDurations = races.size() == 0;
		int index = 0;

		for (const auto word : std::views::split(line, ' '))
		{
			const std::string token(&*word.begin(), std::ranges::distance(word));

			if (token.size() == 0)
				continue;

			if (!std::isdigit(token.front()))
				continue;

			if (parseDurations)
			{
				races.push_back({});
				races.back().duration = std::stoull(token);
			}
			else
			{
				races[index].recordDistance = std::stoull(token);
				++index;
			}
		}
	}

	return races;
}

static auto loadDataPartTwo(const char* filename)
{
	std::fstream s{ filename, s.in };
	assert(s.is_open());

	Race race;

	while (s.peek() != EOF)
	{
		std::string line;
		std::getline(s, line);
		assert(line.size() > 0);

		const bool parseDurations = race.duration == 0;

		for (const auto word : std::views::split(line, ' '))
		{
			const std::string token(&*word.begin(), std::ranges::distance(word));

			if (token.size() == 0)
				continue;

			if (!std::isdigit(token.front()))
				continue;

			const uint64_t number = std::stoull(token);
			const uint64_t numDigits = (uint64_t) log10(number) + 1;

			if (parseDurations)
			{
				race.duration *= (uint64_t) pow(10, numDigits);
				race.duration += number;
			}
			else
			{
				race.recordDistance *= (uint64_t) pow(10, numDigits);
				race.recordDistance += number;
			}
		}
	}

	return race;
}

static uint64_t partOne(const char* filename)
{
	const auto races = loadDataPartOne(filename);

	return std::accumulate(
		begin(races),
		end(races),
		1ull,
		[] (uint64_t sum, const Race& race) { return sum * race.calculateNumWaysOfWinning(); }
	);
}

static uint64_t partTwo(const char* filename)
{
	const auto race = loadDataPartTwo(filename);
	return race.calculateNumWaysOfWinning();
}

static std::pair<uint64_t, uint64_t> process(const char* filename)
{
	return std::make_pair(partOne(filename), partTwo(filename));
}

static void processPrintAndAssert(const char* filename, std::optional<std::pair<uint64_t, uint64_t>> expected = {})
{
	const auto result = process(filename);
	std::cout << "Part 1: " << result.first << " Part 2: " << result.second << std::endl;

	assert(!expected || expected.value().first == 0 || result.first == expected.value().first);
	assert(!expected || expected.value().second == 0 || result.second == expected.value().second);
}

void day06()
{
	processPrintAndAssert("../data/06/test.txt", std::make_pair(288ull, 71503ull));
	processPrintAndAssert("../data/06/real.txt", std::make_pair(608902ull, 46173809ull));
}
