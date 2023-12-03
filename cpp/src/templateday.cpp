#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
#include <string>
#include <vector>

static auto loadData(const char* filename)
{
	std::fstream s{ filename, s.in };
	assert(s.is_open());

	std::vector<std::string> lines;

	while (s.peek() != EOF)
	{
		lines.push_back("");
		std::string& line = lines.back();
		std::getline(s, line);
		assert(line.size() > 0);
	}

	return lines;
}

static uint64_t partOne(const auto& data)
{
	return std::accumulate(
		begin(data),
		end(data),
		0ull,
		[] (uint64_t sum, const auto& line) { return sum + line.size(); }
	);
}

static uint64_t partTwo(const auto& data)
{
	return std::accumulate(
		begin(data),
		end(data),
		0ull,
		[] (uint64_t sum, const auto& line) { return sum + line.size(); }
	);
}

static std::pair<uint64_t, uint64_t> process(const char* filename)
{
	const auto data = loadData(filename);
	return std::make_pair(partOne(data), partTwo(data));
}

static void processPrintAndAssert(const char* filename, std::optional<std::pair<uint64_t, uint64_t>> expected = {})
{
	const auto result = process(filename);
	std::cout << "Part 1: " << result.first << " Part 2: " << result.second << std::endl;

	assert(!expected || expected.value().first == 0 || result.first == expected.value().first);
	assert(!expected || expected.value().second == 0 || result.second == expected.value().second);
}

void day_X_()
{
	processPrintAndAssert("../data/_X_/test.txt", std::make_pair(0ull, 0ull));
	processPrintAndAssert("../data/_X_/real.txt", std::make_pair(0ull, 0ull));
}
