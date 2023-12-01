#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

static const std::vector<std::pair<std::string, int>> numberStrings =
{
	{ "0", 0 },
	{ "1", 1 },
	{ "2", 2 },
	{ "3", 3 },
	{ "4", 4 },
	{ "5", 5 },
	{ "6", 6 },
	{ "7", 7 },
	{ "8", 8 },
	{ "9", 9 },
	{ "zero",   0 },
	{ "one",    1 },
	{ "two",    2 },
	{ "three",  3 },
	{ "four",   4 },
	{ "five",   5 },
	{ "six",    6 },
	{ "seven",  7 },
	{ "eight",  8 },
	{ "nine",   9 },
};

enum class ConsiderText
{
	False,
	True
};

static int process(const char* filename, ConsiderText considerTextMode)
{
	std::fstream s{filename, s.in};
	assert(s.is_open());

	int sum = 0;

	while (s.peek() != EOF)
	{
		std::string line;
		std::getline(s, line);
		assert(line.size() > 0);

		std::vector<std::pair<int, size_t>> numbersAtPositions;

		for (const auto& toFind : numberStrings)
		{
			if (considerTextMode == ConsiderText::False && toFind.first.size() != 1)
				continue;

			const size_t firstPos = line.find(toFind.first);
			if (firstPos == std::string::npos)
				continue;

			numbersAtPositions.emplace_back(toFind.second, firstPos);

			const size_t lastPos = line.rfind(toFind.first);
			if (firstPos != lastPos)
				numbersAtPositions.emplace_back(toFind.second, lastPos);
		}

		std::sort(begin(numbersAtPositions), end(numbersAtPositions), [] (const auto& lhs, const auto& rhs)
		{
			return lhs.second < rhs.second;
		});

		const int first = numbersAtPositions.front().first;
		const int last = numbersAtPositions.back().first;

		const int num = (first * 10) + last;
		sum += num;
	}

	return sum;
}

void day01()
{
	assert(process("../data/01/test.txt", ConsiderText::False) == 142);
	assert(process("../data/01/real.txt", ConsiderText::False) == 55816);
	assert(process("../data/01/test2.txt", ConsiderText::True) == 281);
	assert(process("../data/01/real.txt", ConsiderText::True) == 54980);
}
