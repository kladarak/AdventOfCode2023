#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

static std::vector<std::string> loadSchematic(const char* filename)
{
	std::fstream s{ filename, s.in };
	assert(s.is_open());

	std::vector<std::string> schematic;
	std::optional<size_t> width;

	while (s.peek() != EOF)
	{
		std::string line;
		std::getline(s, line);

		assert(line.size() > 0);
		assert(line.back() != '\n');

		schematic.push_back(line);

		if (width)
			assert(line.size() == width.value());
		else
			width = line.size();
	}

	return schematic;
}

static uint64_t partOne(const std::vector<std::string>& schematic)
{
	const size_t width = schematic.front().size();

	auto hasSymbolNeighbour = [&] (const size_t row, size_t col, size_t mag)
	{
		bool foundSymbol = false;

		for (size_t x = col == 0 ? col : col - 1;
			x < std::min(col + mag + 1, width);
			++x)
		{
			for (size_t y = row == 0 ? row : row - 1;
				y < std::min(row + 2, schematic.size());
				++y)
			{
				const char c = schematic[y][x];
				foundSymbol |= !std::isdigit(c) && c != '.';
			}
		}

		return foundSymbol;
	};

	uint64_t sum = 0;

	for (size_t row = 0; row < schematic.size(); ++row)
	{
		for (size_t col = 0; col < width; ++col)
		{
			char c = schematic[row][col];
			if (!std::isdigit(c))
				continue;

			int num = 0;
			size_t mag = 0;

			for (; col < width; ++col)
			{
				c = schematic[row][col];
				if (!std::isdigit(c))
					break;

				num *= 10;
				num += c - '0';
				++mag;
			}

			if (hasSymbolNeighbour(row, col - mag, mag))
				sum += num;
		}
	}

	return sum;
}

struct Symbol
{
	char symbol = 0;
	size_t x = 0;
	size_t y = 0;
};

struct Part
{
	uint64_t value = 0;
	size_t mag = 0;
	size_t x = 0;
	size_t y = 0;
};

static uint64_t partTwo([[maybe_unused]]  const std::vector<std::string>& schematic)
{
	const size_t width = schematic.front().size();

	std::vector<Part> parts;
	std::vector<std::vector<Symbol>> symbolsPerRow;
	symbolsPerRow.resize(schematic.size());

	for (size_t row = 0; row < schematic.size(); ++row)
	{
		for (size_t col = 0; col < width; ++col)
		{
			const char c = schematic[row][col];
			if (std::isdigit(c))
			{
				const uint64_t partNum = std::stoull(schematic[row].substr(col));
				parts.push_back({ partNum, 0, col, row });

				while (col < width && std::isdigit(schematic[row][col]))
				{
					++parts.back().mag;
					++col;
				}

				--col;
			}
			else if (c != '.')
			{
				symbolsPerRow[row].push_back({c, col, row});
			}
		}
	}

	std::unordered_map<const Symbol*, std::vector<const Part*>> symbolPartMap;

	for (const Part& part : parts)
	{
		const size_t minX = part.x == 0 ? part.x : part.x - 1;
		const size_t maxX = std::min(part.x + part.mag, width - 1);
		const size_t minY = part.y == 0 ? part.y : part.y - 1;
		const size_t maxY = std::min(part.y + 1, schematic.size() - 1);

		for (size_t y = minY; y <= maxY; ++y)
		{
			for (const Symbol& symbol : symbolsPerRow[y])
			{
				if (minX <= symbol.x && symbol.x <= maxX)
					symbolPartMap[&symbol].push_back(&part);
			}
		}
	}

	uint64_t sum = 0;

	for (auto pair : symbolPartMap)
	{
		if (pair.first->symbol == '*' && pair.second.size() == 2)
		{
			const uint64_t product = pair.second[0]->value * pair.second[1]->value;
			sum += product;
		}
	}

	return sum;
}

static std::pair<uint64_t, uint64_t> process(const char* filename)
{
	const auto schematic = loadSchematic(filename);
	return std::make_pair(partOne(schematic), partTwo(schematic));
}

static void processPrintAndAssert(const char* filename, std::optional<std::pair<uint64_t, uint64_t>> expected = {})
{
	const auto result = process(filename);
	std::cout << "Part 1: " << result.first << " Part 2: " << result.second << std::endl;
	assert(!expected || result.first == expected.value().first || expected.value().first == 0);
	assert(!expected || result.second == expected.value().second || expected.value().second == 0);
}

void day03()
{
	processPrintAndAssert("../data/03/test.txt", std::make_pair(4361ull, 467835ull));
	processPrintAndAssert("../data/03/real.txt", std::make_pair(539713ull, 0ull));
}
