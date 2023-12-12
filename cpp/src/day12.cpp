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

	struct Group
	{
		size_t index = 0;
		int pos = 0;
		int size = 0;
	};

	struct SpringRow
	{
		using Permutation = std::vector<CellState>;

	public:
		// input
		Permutation basePermutation;
		std::vector<Group> groups;

		// output
		std::vector<Permutation> validPermutations;

	private:
		std::vector<std::vector<int>> possibleGroupPositions;

	public:
		void evaluatePermutations()
		{
			possibleGroupPositions.resize(groups.size());

			// init group sizes
			{
				const int numFilled =
					std::accumulate(
						begin(groups),
						end(groups),
						0ul,
						[] (int sum, const auto& g) { return sum + g.size; }
				);

				const int minRowSize = numFilled + (int) groups.size() - 1;
				const int freedom = (int) basePermutation.size() - minRowSize;

				int pos = 0;
				for (size_t i = 0; i < groups.size(); ++i)
				{
					auto& groupPositions = possibleGroupPositions[i];
					groupPositions.resize(freedom + 1);
					std::iota(groupPositions.begin(), groupPositions.end(), pos);

					assert(groupPositions.back() < basePermutation.size());
					pos += groups[i].size + 1; // +1 to leave a gap
				}
			}

			// Remove invalid possible positions
			{
				for (size_t i = 0; i < groups.size(); ++i)
				{
					auto& groupPositions = possibleGroupPositions[i];
					Group& group = groups[i];

					std::erase_if(
						groupPositions,
						[&] (int trialPos)
						{
							auto beginIter = basePermutation.begin() + trialPos;
							auto endIter = beginIter + group.size;
							return std::find_if(
								beginIter,
								endIter,
								[] (CellState s) { return s == CellState::Empty; }
							) != endIter;
						}
					);

					assert(groupPositions.size() > 0);
					group.pos = groupPositions.front();
				}
			}
			
			// Build and check permutations
			{
				std::function<void (const std::vector<Group>&)> permutate;
				permutate = [&] (const std::vector<Group>& permutation)
				{
					if (permutation.size() == groups.size())
					{
						// Here we have a filled permutation - convert into cell state and push it.
						Permutation permCellState(basePermutation.size(), CellState::Empty);

						for (const Group& g : permutation)
						{
							for (int p = g.pos; p < g.pos + g.size; ++p)
								permCellState[p] = CellState::Filled;
						}

						bool isValid = true;
						for (size_t i = 0; i < permCellState.size(); ++i)
						{
							isValid &= (permCellState[i] == basePermutation[i]
								|| basePermutation[i] == CellState::Unknown);
						}

						if (isValid)
							validPermutations.push_back(std::move(permCellState));
					}
					else
					{
						// Add this group and recurse.
						const size_t index = permutation.size();
						const int startPos = index == 0 ? 0 : permutation.back().pos + permutation.back().size + 1;

						auto nextPerm = permutation;
						nextPerm.push_back(groups[index]);

						for (int pos : possibleGroupPositions[index])
						{
							if (pos >= startPos)
							{
								nextPerm.back().pos = pos;
								permutate(nextPerm);
							}
						}
					}
				};
				permutate({});
			}
		}
	};

	struct Springs
	{
		std::vector<SpringRow> rows;

		Springs() = default;

		explicit Springs(const char* filename)
		{
			load(filename);
		}

		void load(const char* filename)
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
				
				auto viewToString = [] (auto const v) { return std::string(&*v.begin(), std::ranges::distance(v)); };
				for (const std::string& num : std::views::split(groupsStr, ',') | std::views::transform(viewToString))
				{
					Group g;
					g.index = row.groups.size();
					g.size = std::stoi(num);
					row.groups.push_back(g);
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
			[] (uint64_t sum, const SpringRow& row) { return sum + row.validPermutations.size(); }
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

				newRow.groups.insert(
					newRow.groups.end(),
					oldRow.groups.begin(),
					oldRow.groups.end()
				);
			}
		}

		springs.evaluatePermutations();

		return std::accumulate(
			begin(springs.rows),
			end(springs.rows),
			0ull,
			[] (uint64_t sum, const SpringRow& row) { return sum + row.validPermutations.size(); }
		);
	}

	static std::pair<uint64_t, uint64_t> process(const char* filename)
	{
		Springs springs(filename);
		return std::make_pair(partOne(springs), partTwo(springs));
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
	processPrintAndAssert("../data/12/real.txt", std::make_pair(7025ull, 0ull));
}
