#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <functional>
#include <map>
#include <numeric>
#include <optional>
#include <ranges>
#include <set>
#include <string>
#include <vector>
#include <unordered_map>

namespace d13
{
	struct LavaMap
	{
		std::vector<std::string> cells;
		size_t indexOfSymmetry = SIZE_MAX;

		void validate() const
		{
			for (auto& line : cells)
				assert(line.size() == cells.front().size());
		}

		void evalLineOfSymmetry()
		{
			for (size_t i = 1; i < cells.size() && indexOfSymmetry == SIZE_MAX; ++i)
			{
				const size_t sum = i + i - 1;
				bool foundSymmetry = true;

				for (size_t j = i; j < cells.size() && j <= sum && foundSymmetry; ++j)
				{
					const size_t k = sum - j;
					foundSymmetry &= (cells[j] == cells[k]);
				}

				if (foundSymmetry)
					indexOfSymmetry = i;
			}
		}

		void fixSmudgeAndEvalSymmetry()
		{
			for (size_t i = 1; i < cells.size() && indexOfSymmetry == SIZE_MAX; ++i)
			{
				const size_t sum = i + i - 1;
				std::pair<size_t, size_t> smudgePos;
				int numDifferences = 0;

				for (size_t j = i; j < cells.size() && j <= sum && numDifferences <= 1; ++j)
				{
					const size_t k = sum - j;

					for (size_t c = 0; c < cells[j].size() && numDifferences <= 1; ++c)
					{
						if (cells[j][c] != cells[k][c])
						{
							smudgePos = std::make_pair(j, c);
							++numDifferences;
						}
					}
				}

				if (numDifferences == 1)
				{
					char& c = cells[smudgePos.first][smudgePos.second];
					c = c == '#' ? '.' : '#';
					indexOfSymmetry = i;
				}
			}
		}
	};

	struct LavaMaps
	{
		LavaMap rows;
		LavaMap cols;

		void transposeRowsToCols()
		{
			rows.validate();

			for (size_t col = 0; col < rows.cells.front().size(); ++col)
			{
				cols.cells.push_back({});

				for (const std::string& cells : rows.cells)
				{
					cols.cells.back().push_back(cells[col]);
				}
			}

			cols.validate();
		}

		void evalLinesOfSymmetry()
		{
			rows.evalLineOfSymmetry();
			cols.evalLineOfSymmetry();

			assert(rows.indexOfSymmetry != SIZE_MAX || cols.indexOfSymmetry != SIZE_MAX);
		}

		void fixSmudgesAndEvalSymmetry()
		{
			rows.fixSmudgeAndEvalSymmetry();
			cols.fixSmudgeAndEvalSymmetry();

			assert(rows.indexOfSymmetry != SIZE_MAX || cols.indexOfSymmetry != SIZE_MAX);
		}
	};

	struct AllLavaMaps
	{
		std::vector<LavaMaps> lavaMaps;

		explicit AllLavaMaps(const char* filename)
		{
			loadRows(filename);
			transposeRowsToCols();
		}

		void loadRows(const char* filename)
		{
			std::fstream s{ filename, s.in };
			assert(s.is_open());

			lavaMaps.push_back({});

			while (s.peek() != EOF)
			{
				std::string line;
				std::getline(s, line);

				if (line.size() == 0)
					lavaMaps.push_back({});
				else
					lavaMaps.back().rows.cells.push_back(line);
			}
		}

		void transposeRowsToCols()
		{
			for (auto& maps : lavaMaps)
				maps.transposeRowsToCols();
		}

		void evalLinesOfSymmetry()
		{
			for (auto& maps : lavaMaps)
				maps.evalLinesOfSymmetry();
		}

		void fixSmudgesAndEvalSymmetry()
		{
			for (auto& maps : lavaMaps)
				maps.fixSmudgesAndEvalSymmetry();
		}
	};

	static uint64_t partOne(const AllLavaMaps& allMaps)
	{
		auto copy = allMaps;
		copy.evalLinesOfSymmetry();

		return std::accumulate(
			begin(copy.lavaMaps),
			end(copy.lavaMaps),
			0ull,
			[] (uint64_t sum, const LavaMaps& maps)
			{
				return sum
					+ (maps.rows.indexOfSymmetry != SIZE_MAX
						? 100 * maps.rows.indexOfSymmetry
						: maps.cols.indexOfSymmetry);
			}
		);
	}

	static uint64_t partTwo(const auto& allMaps)
	{
		auto copy = allMaps;
		copy.fixSmudgesAndEvalSymmetry();

		return std::accumulate(
			begin(copy.lavaMaps),
			end(copy.lavaMaps),
			0ull,
			[] (uint64_t sum, const LavaMaps& maps)
			{
				return sum
					+ (maps.rows.indexOfSymmetry != SIZE_MAX
						? 100 * maps.rows.indexOfSymmetry
						: maps.cols.indexOfSymmetry);
			}
		);
	}

	static std::pair<uint64_t, uint64_t> process(const char* filename)
	{
		const AllLavaMaps maps(filename);
		return std::make_pair(partOne(maps), partTwo(maps));
	}

	static void processPrintAndAssert(const char* filename, std::pair<uint64_t, uint64_t> expected)
	{
		const auto result = process(filename);
		std::cout << "Part 1: " << result.first << " Part 2: " << result.second << std::endl;

		assert(expected.first == 0 || result.first == expected.first);
		assert(expected.second == 0 || result.second == expected.second);
	}
}

void day13()
{
	d13::processPrintAndAssert("../data/13/test.txt", std::make_pair(405ull, 400ull));
	d13::processPrintAndAssert("../data/13/real.txt", std::make_pair(34202ull, 34230ull));
}
