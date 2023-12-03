#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

struct Symbol
{
	char symbol = 0;
	size_t x = 0;
	size_t y = 0;
};

struct Part
{
	uint64_t value = 0;
	size_t x = 0;
	size_t y = 0;

	size_t width() const { return static_cast<size_t>(log10(value) + 1); }
	size_t height() const { return 1; }
};

struct Gear
{
	const Part* part1 = nullptr;
	const Part* part2 = nullptr;
	const Symbol* symbol = nullptr;

	uint64_t ratio() const { return part1->value * part2->value; }
};

struct Schematic
{
	std::vector<std::string> data;
	size_t width = 0;
	size_t height = 0;

	std::vector<Part> parts;
	std::vector<Symbol> symbols;
	std::vector<Gear> gears;

	std::vector<const Part*> partsNeighbouringSymbols;
	std::unordered_map<const Symbol*, std::vector<const Part*>> symbolPartMap;

	explicit Schematic(const char* filename)
	{
		loadData(filename);
		identifyPartsAndSynbols();
		buildPartSymbolMap();
		identifyGears();
	}

	void loadData(const char* filename)
	{
		std::fstream s{ filename, s.in };
		assert(s.is_open());

		while (s.peek() != EOF)
		{
			std::string line;
			std::getline(s, line);

			assert(line.size() > 0);
			assert(line.back() != '\n');

			data.push_back(line);

			if (width != 0)
				assert(line.size() == width);
			else
				width = line.size();
		}

		height = data.size();
	}
	
	void identifyPartsAndSynbols()
	{
		for (size_t row = 0; row < height; ++row)
		{
			for (size_t col = 0; col < width; ++col)
			{
				const char c = data[row][col];
				if (std::isdigit(c))
				{
					const uint64_t partNum = std::stoull(data[row].substr(col));
					parts.push_back({ partNum, col, row });
					col += parts.back().width() - 1;
				}
				else if (c != '.')
				{
					symbols.push_back({ c, col, row });
				}
			}
		}
	}

	void buildPartSymbolMap()
	{
		std::vector<std::vector<const Symbol*>> symbolsPerRow;
		symbolsPerRow.resize(height);

		for (const Symbol& s : symbols)
			symbolsPerRow[s.y].push_back(&s);

		for (const Part& part : parts)
		{
			const size_t minX = part.x == 0 ? part.x : part.x - 1;
			const size_t maxX = std::min(part.x + part.width(), width - 1);
			const size_t minY = part.y == 0 ? part.y : part.y - 1;
			const size_t maxY = std::min(part.y + part.height(), height - 1);

			for (size_t y = minY; y <= maxY; ++y)
			{
				for (const Symbol* symbol : symbolsPerRow[y])
				{
					if (minX <= symbol->x && symbol->x <= maxX)
					{
						partsNeighbouringSymbols.push_back(&part);
						symbolPartMap[symbol].push_back(&part);
					}
				}
			}
		}
	}

	void identifyGears()
	{
		for (auto pair : symbolPartMap)
			if (pair.first->symbol == '*' && pair.second.size() == 2)
				gears.push_back({ pair.second[0], pair.second[1], pair.first });
	}
};

static uint64_t partOne(const Schematic& schematic)
{
	return std::accumulate(
		begin(schematic.partsNeighbouringSymbols),
		end(schematic.partsNeighbouringSymbols),
		0ull,
		[] (uint64_t sum, const Part* part) { return sum + part->value; }
	);
}

static uint64_t partTwo(const Schematic& schematic)
{
	return std::accumulate(
		begin(schematic.gears),
		end(schematic.gears),
		0ull,
		[] (uint64_t sum, const Gear& gear) { return sum + gear.ratio(); }
	);
}

static std::pair<uint64_t, uint64_t> process(const char* filename)
{
	const Schematic schematic(filename);
	return std::make_pair(partOne(schematic), partTwo(schematic));
}

static void processPrintAndAssert(const char* filename, std::optional<std::pair<uint64_t, uint64_t>> expected = {})
{
	const auto result = process(filename);
	std::cout << "Part 1: " << result.first << " Part 2: " << result.second << std::endl;

	assert(!expected || expected.value().first == 0 || result.first == expected.value().first);
	assert(!expected || expected.value().second == 0 || result.second == expected.value().second);
}

void day03()
{
	processPrintAndAssert("../data/03/test.txt", std::make_pair(4361ull, 467835ull));
	processPrintAndAssert("../data/03/real.txt", std::make_pair(539713ull, 84159075ull));
}
