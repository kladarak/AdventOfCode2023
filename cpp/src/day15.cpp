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

namespace d15
{
	static uint64_t lavaHash(const std::string& str)
	{
		return std::accumulate(
			begin(str),
			end(str),
			0ull,
			[] (uint64_t sum, const char& c) { return ((sum + c) * 17) % 256; }
		);
	}

	struct Lens
	{
		std::string label;
		uint64_t focalLength = 0;

		uint64_t hash() const { return lavaHash(label); }
	};

	struct LensFocuser
	{
		std::vector<std::string> instructions;
		std::vector<std::vector<Lens>> boxes;

		explicit LensFocuser(const char* filename)
		{
			loadInstructions(filename);
			fillBoxes();
		}

		void loadInstructions(const char* filename)
		{
			std::fstream s{ filename, s.in };
			assert(s.is_open());

			std::string line;
			std::getline(s, line);
			assert(line.size() > 0);

			for (const auto word : std::views::split(line, ','))
				instructions.emplace_back(&*word.begin(), std::ranges::distance(word));
		}

		void fillBoxes()
		{
			boxes.resize(256);

			for (const std::string& instruction : instructions)
			{
				if (instruction.back() == '-')
				{
					const std::string label = instruction.substr(0, instruction.size() - 1);
					const uint64_t hash = lavaHash(label);
					auto& box = boxes[hash];

					std::erase_if(box, [&label] (const Lens& l) { return l.label == label;  });
				}
				else
				{
					const size_t pos = instruction.find('=');
					assert(pos != std::string::npos);

					const std::string label = instruction.substr(0, pos);
					const std::string focalLengthStr = instruction.substr(pos + 1);

					const uint64_t hash = lavaHash(label);
					const uint64_t focalLength = std::stoull(focalLengthStr);

					auto& box = boxes[hash];

					auto hasLabel = [&label] (const Lens& lens) { return lens.label == label; };
					if (auto iter = std::find_if(begin(box), end(box), hasLabel); iter != end(box))
						(*iter).focalLength = focalLength;
					else
						boxes[hash].push_back({label, focalLength});
				}
			}
		}

		uint64_t calculateFocusingPower() const
		{
			uint64_t power = 0;

			for (size_t i = 0; i < boxes.size(); ++i)
			{
				for (size_t j = 0; j < boxes[i].size(); ++j)
				{
					power += (i + 1) * (j + 1) * boxes[i][j].focalLength;
				}
			}

			return power;
		}
	};

	static uint64_t partOne(const LensFocuser& f)
	{
		return std::accumulate(
			begin(f.instructions),
			end(f.instructions),
			0ull,
			[] (uint64_t sum, const auto& line) { return sum + lavaHash(line); }
		);
	}

	static uint64_t partTwo(const LensFocuser& f)
	{
		return f.calculateFocusingPower();
	}

	static std::pair<uint64_t, uint64_t> process(const char* filename)
	{
		const LensFocuser data(filename);
		return std::make_pair(partOne(data), partTwo(data));
	}

	static void processPrintAndAssert(const char* filename, std::pair<uint64_t, uint64_t> expected)
	{
		const auto result = process(filename);
		std::cout << "Part 1: " << result.first << " Part 2: " << result.second << std::endl;

		assert(expected.first == 0 || result.first == expected.first);
		assert(expected.second == 0 || result.second == expected.second);
	}
}

void day15()
{
	d15::processPrintAndAssert("../data/15/test.txt", std::make_pair(1320ull, 145ull));
	d15::processPrintAndAssert("../data/15/real.txt", std::make_pair(503487ull, 0ull));
}
