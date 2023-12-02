#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

struct Set
{
	uint64_t blue = 0;
	uint64_t red = 0;
	uint64_t green = 0;
};

struct Game
{
	uint64_t id = 0;
	std::vector<Set> sets;
};

Game parseGame(const std::string& gameString)
{
	std::vector<std::string> tokens;

	{
		size_t pos = 0;

		while (true)
		{
			size_t nextPos = gameString.find(' ', pos);
			if (nextPos != std::string::npos)
			{
				tokens.push_back(gameString.substr(pos, nextPos - pos));
				pos = nextPos + 1;
			}
			else
			{
				tokens.push_back(gameString.substr(pos));
				break;
			}
		}
	}

	assert(tokens.front() == "Game");

	Game game;
	game.id = std::stoull(tokens[1]);
	game.sets.push_back({});

	uint64_t currentNum = 0;

	for (size_t i = 2; i < tokens.size(); ++i)
	{
		const auto& token = tokens[i];

		if (std::isdigit(token.front()))
		{
			currentNum = std::stoull(token);
		}
		else
		{
			Set& set = game.sets.back();

			if (token.find("red") != std::string::npos)
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
	}

	return game;
}

std::vector<Game> loadGames(const char* filename)
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

uint64_t partOne(const std::vector<Game>& games)
{
	uint64_t sum = 0;

	static constexpr uint64_t maxNumRed = 12;
	static constexpr uint64_t maxNumGreen = 13;
	static constexpr uint64_t maxNumBlue = 14;

	for (const Game& game : games)
	{
		bool valid = true;
		for (const Set& set : game.sets)
		{
			valid &= set.red <= maxNumRed
				&& set.green <= maxNumGreen
				&& set.blue <= maxNumBlue;
		}

		if (valid)
			sum += game.id;
	}

	return sum;
}

uint64_t partTwo(const std::vector<Game>& games)
{
	uint64_t sum = 0;

	for (const Game& game : games)
	{
		uint64_t maxRed = 0;
		uint64_t maxGreen = 0;
		uint64_t maxBlue = 0;

		for (const Set& set : game.sets)
		{
			maxRed = std::max(maxRed, set.red);
			maxGreen = std::max(maxGreen, set.green);
			maxBlue = std::max(maxBlue, set.blue);
		}

		const uint64_t power = maxRed * maxGreen * maxBlue;
		sum += power;
	}

	return sum;
}

static auto process(const char* filename)
{
	const std::vector<Game> games = loadGames(filename);
	const uint64_t partOneResult = partOne(games);
	const uint64_t partTwoResult = partTwo(games);
	return std::make_pair(partOneResult, partTwoResult);
}

void day02()
{
	assert(process("../data/02/test.txt") == std::make_pair((uint64_t) 8, (uint64_t) 2286));

	const auto result = process("../data/02/real.txt");
	std::cout << "Part One: " << result.first << " Part Two: " << result.second << std::endl;
}
