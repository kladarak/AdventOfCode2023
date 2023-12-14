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

namespace d14
{
	struct Panel
	{
		std::vector<std::string> rows;
		std::vector<std::string> buffer;

		bool operator<(const Panel& rhs) const { return rows < rhs.rows; }
		bool operator==(const Panel& rhs) const { return rows == rhs.rows; }

		explicit Panel(const char* filename)
		{
			std::fstream s{ filename, s.in };
			assert(s.is_open());

			while (s.peek() != EOF)
			{
				std::string line;
				std::getline(s, line);
				assert(line.size() > 0);
				rows.push_back(line);
			}

			buffer = rows;
		}

		void tiltNorth()
		{
			std::vector<size_t> empty(rows.front().size(), 0);

			for (size_t row = 0; row < rows.size(); ++row)
			{
				const std::string& line = rows[row];

				for (size_t col = 0; col < line.size(); ++col)
				{
					switch (line[col])
					{
						case '.':
							break;

						case '#':
							empty[col] = row + 1;
							break;

						case 'O':
						{
							size_t& emptyRow = empty[col];
							std::swap(rows[emptyRow][col], rows[row][col]);
							++emptyRow;
							break;
						}
					}
				}
			}
		}

		void rotateClockwise90()
		{
			assert(rows.size() == rows.front().size());
			const size_t len = rows.size();

			for (size_t row = 0; row < len; ++row)
				for (size_t col = 0; col < len; ++col)
					buffer[col][len - row - 1] = rows[row][col];

			std::swap(rows, buffer);
		}

		void spinCycle()
		{
			for (int i = 0; i < 4; ++i)
			{
				tiltNorth();
				rotateClockwise90();
			}
		}

		uint64_t calculateLoad()
		{
			uint64_t load = 0;

			const size_t len = rows.size();

			for (size_t row = 0; row < len; ++row)
				for (size_t col = 0; col < len; ++col)
					if (rows[row][col] == 'O')
						load += len - row;

			return load;
		}

		void print()
		{
			for (auto& row : rows)
				std::cout << row << std::endl;

			std::cout << std::endl;
		}
	};

	static uint64_t partOne(const Panel& panel)
	{
		Panel p(panel);
		p.tiltNorth();
		return p.calculateLoad();
	}

	static uint64_t partTwo(const Panel& panel)
	{
		Panel p = panel;

		std::vector<Panel> uniquePanels;

		auto repeatIter = end(uniquePanels);
		bool repeats = false;

		const size_t numIters = 1000000000ull;

		for (size_t i = 0; i < numIters && !repeats; ++i)
		{
			uniquePanels.push_back(p);
			p.spinCycle();

			repeatIter = std::find(begin(uniquePanels), end(uniquePanels), p);
			repeats = repeatIter != end(uniquePanels);
		}

		const size_t loopStart = repeatIter - begin(uniquePanels);
		const size_t loopSize = uniquePanels.size() - loopStart;

		std::cout << "Repeats after " << loopSize
			<< " cycles looping back to " << loopStart
			<< std::endl;

		const size_t finalPanelIndex = ((numIters - loopStart) % loopSize) + loopStart;
		return uniquePanels[finalPanelIndex].calculateLoad();
	}

	static void processPrintAndAssert(const char* filename, std::pair<uint64_t, uint64_t> expected)
	{
		const Panel panel(filename);
		const uint64_t pt1Result = partOne(panel);
		const uint64_t pt2Result = partTwo(panel);
		std::cout << "Part 1: " << pt1Result << " Part 2: " << pt2Result << std::endl;

		assert(expected.first == 0 || pt1Result == expected.first);
		assert(expected.second == 0 || pt2Result == expected.second);
	}
}

void day14()
{
	d14::processPrintAndAssert("../data/14/test.txt", std::make_pair(136ull, 64ull));
	d14::processPrintAndAssert("../data/14/real.txt", std::make_pair(107951ull, 0ull));
}
