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

namespace d12
{
	enum class CellState
	{
		Unknown = -1,
		Empty,
		Filled,
	};

	struct SpringRow
	{
	public:
		// input
		std::vector<CellState> basePermutation;
		std::vector<int> groupSizes;

		// output
		uint64_t numPermutations = 0;

	private:
		std::map<std::pair<size_t, size_t>, uint64_t> memoized;

	public:
		void evaluatePermutations()
		{
			numPermutations = accumulatePermutationsUntilNextFilled(0, 0);
		}

	private:
		uint64_t accumulatePermutationsUntilNextFilled(size_t groupIndex, size_t startPos)
		{
			uint64_t permCount = 0;

			for (size_t pos = startPos; pos < basePermutation.size(); ++pos)
			{
				permCount += permutate(groupIndex, pos);

				// Cannot skip cells that must be filled.
				if (basePermutation[pos] == CellState::Filled)
					break;
			}

			return permCount;
		}

		uint64_t permutate(const size_t groupIndex, const size_t pos)
		{
			const auto groupPoskey = std::make_pair(groupIndex, pos);

			if (auto iter = memoized.find(groupPoskey); iter != memoized.end())
				return iter->second;

			uint64_t permCount = 0;

			if (canGroupFitAtPosition(groupIndex, pos))
			{
				const size_t groupSize = groupSizes[groupIndex];
				const size_t nextPos = pos + groupSize + 1;
				const size_t nextGroupIndex = groupIndex + 1;

				if (nextGroupIndex < groupSizes.size())
				{
					// Recurse into next group if we have more groups to process
					permCount = accumulatePermutationsUntilNextFilled(nextGroupIndex, nextPos);
				}
				else if (findNextMustFillCellIndex(nextPos) == SIZE_MAX)
				{
					// Only consider this a valid permutation if there are no more cells that must be filled.
					permCount = 1;
				}
			}

			// Cache so that when this key is queried again we don't need to recurse through all permutations again.
			memoized[groupPoskey] = permCount;

			return permCount;
		}

		bool canGroupFitAtPosition(const size_t groupIndex, const size_t pos) const
		{
			const size_t rowSize = basePermutation.size();
			const size_t groupSize = groupSizes[groupIndex];
			const size_t endPos = pos + groupSize;

			// Cannot fit if group is larger than remaining space in row.
			if (endPos > rowSize)
				return false;

			// Cannot fit if cell that immediately follows must also be filled (groups cannot sit next to each other).
			if ((endPos < rowSize) && basePermutation[endPos] == CellState::Filled)
				return false;

			// Cannot fit if any cells must be empty.
			return std::all_of(
				begin(basePermutation) + pos,
				begin(basePermutation) + endPos,
				[] (CellState s) { return s != CellState::Empty; }
			);
		}

		size_t findNextMustFillCellIndex(const size_t fromPos)
		{
			const auto beginIter = begin(basePermutation);
			const auto endIter = end(basePermutation);
			const auto fromIter = beginIter + fromPos;
			const auto foundIter = std::find(fromIter, endIter, CellState::Filled);
			return foundIter < endIter ? (size_t) (foundIter - beginIter) : SIZE_MAX;
		}
	};

	struct Springs
	{
		std::vector<SpringRow> rows;

		Springs() = default;

		explicit Springs(const char* filename)
		{
			std::fstream s{ filename, s.in };
			assert(s.is_open());

			while (s.peek() != EOF)
			{
				std::string line;
				std::getline(s, line);
				assert(line.size() > 0);

				rows.push_back({});
				SpringRow& row = rows.back();

				const size_t pos = line.find(' ');
				const std::string damagedStr = line.substr(0, pos);
				const std::string groupsStr = line.substr(pos + 1);

				for (const char c : damagedStr)
				{
					row.basePermutation.push_back([c] ()
					{
						switch (c)
						{
							case '?': return CellState::Unknown;
							case '#': return CellState::Filled;
							case '.': return CellState::Empty;
						}
						assert(false);
						return CellState::Unknown;
					} ());
				}
				
				//row.groupSizes
				//	= std::views::split(groupsStr, ',')
				//	| std::views::transform([] (auto const v) { return std::string(&*v.begin(), std::ranges::distance(v)); })
				//	| std::views::transform([] (const std::string& s) { return std::stoi(s);  })
				//	| std::ranges::to<std::vector>(); // requires C++23

				for (const int groupSize : std::views::split(groupsStr, ',')
					| std::views::transform([] (auto const v) { return std::string(&*v.begin(), std::ranges::distance(v)); })
					| std::views::transform([] (const std::string& s) { return std::stoi(s); }))
				{
					row.groupSizes.push_back(groupSize);
				}
			}
		}

		void evaluatePermutations()
		{
			for (auto& row : rows)
				row.evaluatePermutations();
		}
	};

	static uint64_t partOne(const Springs& input)
	{
		Springs springs = input;
		springs.evaluatePermutations();

		return std::accumulate(
			begin(springs.rows),
			end(springs.rows),
			0ull,
			[] (uint64_t sum, const SpringRow& row) { return sum + row.numPermutations; }
		);
	}

	static uint64_t partTwo(const Springs& input)
	{
		Springs springs;

		for (const SpringRow& oldRow : input.rows)
		{
			springs.rows.push_back({});
			SpringRow& newRow = springs.rows.back();

			for (int i = 0; i < 5; ++i)
			{
				if (i != 0)
					newRow.basePermutation.push_back(CellState::Unknown);

				newRow.basePermutation.insert(
					newRow.basePermutation.end(),
					oldRow.basePermutation.begin(),
					oldRow.basePermutation.end()
				);

				newRow.groupSizes.insert(
					newRow.groupSizes.end(),
					oldRow.groupSizes.begin(),
					oldRow.groupSizes.end()
				);
			}
		}

		springs.evaluatePermutations();

		return std::accumulate(
			begin(springs.rows),
			end(springs.rows),
			0ull,
			[] (uint64_t sum, const SpringRow& row) { return sum + row.numPermutations; }
		);
	}

	static std::pair<uint64_t, uint64_t> process(const char* filename)
	{
		const Springs springs(filename);
		const auto pt1 = partOne(springs);
		const auto pt2 = partTwo(springs);
		return std::make_pair(pt1, pt2);
	}
}

static void processPrintAndAssert(const char* filename, std::optional<std::pair<uint64_t, uint64_t>> expected = {})
{
	const auto result = d12::process(filename);
	std::cout << "Part 1: " << result.first << " Part 2: " << result.second << std::endl;

	assert(!expected || expected.value().first == 0 || result.first == expected.value().first);
	assert(!expected || expected.value().second == 0 || result.second == expected.value().second);
}

void day12()
{
	processPrintAndAssert("../data/12/test.txt", std::make_pair(21ull, 525152ull));
	processPrintAndAssert("../data/12/real.txt", std::make_pair(7025ull, 11461095383315ull));
}
