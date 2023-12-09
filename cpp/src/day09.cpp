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

struct Data09
{
	std::vector<std::vector<int>> numberRows;
	std::vector<std::vector<std::vector<int>>> differences;
	std::vector<int> extrapolated;
	std::vector<int> backExtrapolated;

	void resolveDifferences()
	{
		for (auto& numRow : numberRows)
		{
			differences.push_back({});
			auto& differencesForRow = differences.back();

			differencesForRow.push_back(numRow);

			auto allZero = [] (auto& differenceRow)
			{
				return std::all_of(
					begin(differenceRow),
					end(differenceRow),
					[] (int num) { return num == 0; });
			};

			while (!allZero(differencesForRow.back()))
			{
				differencesForRow.push_back({});
				const auto& prevRow = *(differencesForRow.end() - 2);
				auto& nextRow = *(differencesForRow.end() - 1);

				for (int i = 0; i < (int) prevRow.size() - 1; ++i)
				{
					nextRow.push_back(prevRow[i + 1] - prevRow[i]);
				}
			} 
		}
	}

	void resolveExtrapolated()
	{
		for (auto& differencesForRow : differences)
		{
			extrapolated.push_back(0);
			int& extrap = extrapolated.back();

			for (int i = (int) differencesForRow.size() - 1; i >= 0; --i)
			{
				auto& differenceRow = differencesForRow[i];
				extrap += differenceRow.back();
			}
		}
	}

	void resolveBackExtrapolated()
	{
		for (auto& differencesForRow : differences)
		{
			backExtrapolated.push_back(0);
			int& extrap = backExtrapolated.back();

			for (int i = (int) differencesForRow.size() - 1; i >= 0; --i)
			{
				auto& differenceRow = differencesForRow[i];
				extrap = differenceRow.front() - extrap;
			}
		}
	}
};

static auto loadData(const char* filename)
{
	std::fstream s{ filename, s.in };
	assert(s.is_open());

	Data09 data;

	while (s.peek() != EOF)
	{
		std::string line;
		std::getline(s, line);
		assert(line.size() > 0);

		data.numberRows.push_back({});

		for (const auto word : std::views::split(line, ' '))
		{
			const std::string token(&*word.begin(), std::ranges::distance(word));
			data.numberRows.back().push_back(std::stoi(token));
		}
	}

	data.resolveDifferences();
	data.resolveExtrapolated();
	data.resolveBackExtrapolated();
	
	return data;
}

static int partOne(const Data09& data)
{
	return std::accumulate(
		begin(data.extrapolated),
		end(data.extrapolated),
		0,
		[] (int sum, const int extrap) { return sum + extrap; }
	);
}

static int partTwo(const Data09& data)
{
	return std::accumulate(
		begin(data.backExtrapolated),
		end(data.backExtrapolated),
		0,
		[] (int sum, const int extrap) { return sum + extrap; }
	);
}

static std::pair<int, int> process(const char* filename)
{
	const auto data = loadData(filename);
	return std::make_pair(partOne(data), partTwo(data));
}

static void processPrintAndAssert(const char* filename, std::optional<std::pair<int, int>> expected = {})
{
	const auto result = process(filename);
	std::cout << "Part 1: " << result.first << " Part 2: " << result.second << std::endl;

	assert(!expected || expected.value().first == 0 || result.first == expected.value().first);
	assert(!expected || expected.value().second == 0 || result.second == expected.value().second);
}

void day09()
{
	processPrintAndAssert("../data/09/test.txt", std::make_pair(114, 2));
	processPrintAndAssert("../data/09/real.txt", std::make_pair(1772145754, 0));
}
