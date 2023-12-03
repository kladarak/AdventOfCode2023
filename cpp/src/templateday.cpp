#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
#include <string>
#include <vector>

static auto process(const char* filename)
{
	std::fstream s{ filename, s.in };
	assert(s.is_open());

	return std::make_pair(0, 0);
}

static void processPrintAndAssert(const char* filename, std::optional<std::pair<int, int>> expected = {})
{
	const auto result = process(filename);
	std::cout << "Part 1: " << result.first << " Part 2: " << result.second << std::endl;
	assert(!expected || result == expected);
}

void day_X_()
{
	processPrintAndAssert("../data/_X_/test.txt");
	processPrintAndAssert("../data/_X_/real.txt");
}
