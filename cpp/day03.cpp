#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
#include <string>
#include <vector>

static std::pair<uint64_t, uint64_t> process(const char* filename)
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

	auto hasSymbolNeighbour = [&] (const size_t row, size_t col, size_t mag)
	{
		bool foundSymbol = false;

		for (size_t x = col == 0 ? col : col - 1;
			x < std::min(col + mag + 1, width.value());
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
		for (size_t col = 0; col < width.value(); ++col)
		{
			char c = schematic[row][col];
			if (!std::isdigit(c))
				continue;

			int num = 0;
			size_t mag = 0;

			for (; col < width.value(); ++col)
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

	return std::make_pair(sum, 0);
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
