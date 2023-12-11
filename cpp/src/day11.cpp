#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <optional>
#include <ranges>
#include <set>
#include <string>
#include <vector>
#include <unordered_map>

namespace d11
{

struct Vec2
{
	int64_t x = 0;
	int64_t y = 0;

	friend Vec2 operator-(const Vec2& lhs, const Vec2& rhs)
	{
		return {lhs .x - rhs.x, lhs.y - rhs.y};
	}

	static int64_t manhattanDistance(const Vec2& lhs, const Vec2& rhs)
	{
		const Vec2 diff = rhs - lhs;
		return abs(diff.x) + abs(diff.y);
	}
};

struct Galaxy
{
	int64_t id = 0;
	Vec2 pos;
};

struct GalaxyMap
{
	std::vector<std::vector<bool>> grid;
	std::vector<Galaxy> galaxies;
	std::map<std::pair<int64_t, int64_t>, int64_t> manhattanDistancesPt1;
	std::map<std::pair<int64_t, int64_t>, int64_t> manhattanDistancesPt2;

	std::vector<int64_t> emptyRows;
	std::vector<int64_t> emptyCols;

	int64_t pt1EmptyScale = 0;
	int64_t pt2EmptyScale = 0;

	explicit GalaxyMap(const char* filename, int64_t _pt1EmptyScale, int64_t _pt2EmptyScale)
		: pt1EmptyScale(_pt1EmptyScale)
		, pt2EmptyScale(_pt2EmptyScale)
	{
		load(filename);
		expandSpace();
		identifyGalaxies();
		calculateManhattanDistances();
	}

private:
	void load(const char* filename)
	{
		std::fstream s{ filename, s.in };
		assert(s.is_open());

		while (s.peek() != EOF)
		{
			std::string line;
			std::getline(s, line);
			assert(line.size() > 0);

			grid.push_back({});
			auto& row = grid.back();

			for (const char c : line)
				row.push_back(c == '#');
		}

		for (auto& row : grid)
			assert(row.size() == grid.front().size());
	}

	void expandSpace()
	{
		std::set<size_t> populatedRows;
		std::set<size_t> populatedCols;

		for (size_t row = 0; row < grid.size(); ++row)
			for (size_t col = 0; col < grid.front().size(); ++col)
				if (grid[row][col])
					(populatedRows.insert(row), populatedCols.insert(col));

		for (size_t row = 0; row < grid.size(); ++row)
			if (!populatedRows.contains(row))
				emptyRows.push_back((int64_t) row);

		for (size_t col = 0; col < grid.front().size(); ++col)
			if (!populatedCols.contains(col))
				emptyCols.push_back((int64_t) col);

		std::sort(begin(emptyRows), end(emptyRows));
		std::sort(begin(emptyCols), end(emptyCols));
	}

	void identifyGalaxies()
	{
		int64_t id = 0;

		for (size_t row = 0; row < grid.size(); ++row)
			for (size_t col = 0; col < grid.front().size(); ++col)
				if (grid[row][col])
					galaxies.push_back({ id++, Vec2{(int64_t) col, (int64_t) row} });
	}

	void calculateManhattanDistances()
	{
		for (size_t i = 0; i < galaxies.size() - 1; ++i)
		{
			const Galaxy& g1 = galaxies[i];

			for (size_t j = i + 1; j < galaxies.size(); ++j)
			{
				const Galaxy& g2 = galaxies[j];
				assert(g1.id < g2.id);

				const int64_t numEmptyRows = numEmptyCellsBetween(g1.pos.y, g2.pos.y, emptyRows);
				const int64_t numEmptyCols = numEmptyCellsBetween(g1.pos.x, g2.pos.x, emptyCols);
				const int64_t numEmptyCells = numEmptyRows + numEmptyCols;

				const int64_t manDist = Vec2::manhattanDistance(g1.pos, g2.pos);

				manhattanDistancesPt1[{g1.id, g2.id}] = manDist + numEmptyCells * (pt1EmptyScale - 1);
				manhattanDistancesPt2[{g1.id, g2.id}] = manDist + numEmptyCells * (pt2EmptyScale - 1);
			}
		}
	}

	static int64_t numEmptyCellsBetween(int64_t cell1, int64_t cell2, const std::vector<int64_t>& emptyCells)
	{
		const int64_t lowCell = std::min(cell1, cell2);
		const int64_t highCell = std::max(cell1, cell2);

		auto lowestIter = std::lower_bound(begin(emptyCells), end(emptyCells), lowCell);
		auto highestIter = std::lower_bound(begin(emptyCells), end(emptyCells), highCell);
		return (int64_t) (highestIter - lowestIter);
	}
};

static int64_t partOne(const GalaxyMap& galaxyMap)
{
	return std::accumulate(
		begin(galaxyMap.manhattanDistancesPt1),
		end(galaxyMap.manhattanDistancesPt1),
		0ll,
		[] (int64_t sum, const auto& pair) { return sum + pair.second; }
	);
}

static int64_t partTwo(const GalaxyMap& galaxyMap)
{
	return std::accumulate(
		begin(galaxyMap.manhattanDistancesPt2),
		end(galaxyMap.manhattanDistancesPt2),
		0ll,
		[] (int64_t sum, const auto& pair) { return sum + pair.second; }
	);
}

static std::pair<int64_t, int64_t> process(const char* filename, int64_t pt1Scale, int64_t pt2Scale)
{
	const GalaxyMap map(filename, pt1Scale, pt2Scale);
	return std::make_pair(partOne(map), partTwo(map));
}

} // d11

static void processPrintAndAssert(const char* filename, int64_t pt1Scale, int64_t pt2Scale, std::optional<std::pair<int64_t, int64_t>> expected = {})
{
	const auto result = d11::process(filename, pt1Scale, pt2Scale);
	std::cout << "Part 1: " << result.first << " Part 2: " << result.second << std::endl;

	assert(!expected || expected.value().first == 0 || result.first == expected.value().first);
	assert(!expected || expected.value().second == 0 || result.second == expected.value().second);
}

void day11()
{
	processPrintAndAssert("../data/11/test.txt", 2, 100, std::make_pair(374, 8410));
	processPrintAndAssert("../data/11/real.txt", 2, 1'000'000, std::make_pair(9599070, 842645913794));
}
