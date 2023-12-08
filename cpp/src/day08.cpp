#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
#include <ranges>
#include <set>
#include <string>
#include <vector>
#include <unordered_map>

struct Node08
{
	std::string left;
	std::string right;
};

enum class Instruction08
{
	Left,
	Right
};

struct NumSteps
{
	struct Stats
	{
		std::vector<std::pair<uint64_t, uint64_t>> seen;
	};

	std::string from;
	std::string to;

	std::vector<std::pair<uint64_t, uint64_t>> stepCounts;
	std::string startCycleKey;
	uint64_t startCycleInstructionIndex = 0;
	uint64_t startCycleStepCount = 0;

	std::unordered_map<std::string, Stats> nodeStats;
	
	void print() const
	{
		std::cout << from << " to " << to << ": ";

		if (stepCounts.size() == 0)
			std::cout << "Unreachable";
		else
			for (auto& stepCount : stepCounts)
				std::cout << "[" << stepCount.first << ", " << stepCount.second << "]";

		std::cout << " Cycle start: { "
			<< "key: " << startCycleKey << ", "
			<< "instruction: " << startCycleInstructionIndex << ", "
			<< "step count at cycle: " << startCycleStepCount;

		//if (stepCounts.size() != 0)
		//	if (nodeStats.contains(startCycleKey))
		//		for (auto& seen : nodeStats.at(startCycleKey).seen)
		//			std::cout << "[" << seen.first << ", " << seen.second << "]";

		std::cout << " }";

		std::cout << std::endl;
	}

	bool isReachable() const
	{
		return stepCounts.size() > 0;
	}
};

struct Graph08
{
	std::unordered_map<std::string, Node08> nodes;
	std::vector<Instruction08> instructions;

	NumSteps numStepsUntilCycle(const std::string& start, const std::string& end, uint64_t instructionOffset = 0) const
	{
		NumSteps output = { start, end };

		if (!nodes.contains(start))
			return output;

		if (!nodes.contains(end))
			return output;

		std::string current = start;
		uint64_t steps = 0;
		uint64_t index = instructionOffset;

		std::set<std::pair<std::string, uint64_t>> visited;

		while (visited.insert(std::make_pair(current, index)).second)
		{
			output.nodeStats[current].seen.emplace_back(index, steps);

			const Node08& node = nodes.at(current);
			const bool takeLeft = instructions[index] == Instruction08::Left;
			current = takeLeft ? node.left : node.right;
			++steps;
			index = (index + 1) % instructions.size();

			if (current == end)
				output.stepCounts.emplace_back(index, steps);
		}

		output.startCycleKey = current;
		output.startCycleInstructionIndex = index;
		output.startCycleStepCount = steps;

		return output;
	}

	uint64_t numStepsPartOne() const
	{
		auto steps = numStepsUntilCycle("AAA", "ZZZ").stepCounts;
		return steps.size() > 0 ? steps.front().second : UINT64_MAX;
	}

	uint64_t partTwoNumSteps() const
	{
		/*
		* This is too slow.
		* This needs to be more of a common factor solution.
		* For each starting node, find out the number of steps to get to each Z node.
		* The num steps will be the point at which all paths end on the same Z..
		* 
		* A: __Z____Z__Z___Z
		* B: _Z___Z___Z_Z
		* C: ___Z_Z___Z___Z
		* 
		* Loop above until you find a column which has Z in all of them.
		*/

		std::vector<std::string> aKeys;
		std::vector<std::string> zKeys;

		for (auto& iter : nodes)
		{
			if (iter.first.back() == 'A')
				aKeys.push_back(iter.first);
			if (iter.first.back() == 'Z')
				zKeys.push_back(iter.first);
		}

		assert(aKeys.size() <= zKeys.size());

		std::cout << "--- Part Two Analysis ---" << std::endl;
		std::cout << "Instruction count: " << instructions.size() << std::endl;

		std::vector<NumSteps> reachable;

		for (const std::string& a : aKeys)
		{
			numStepsUntilCycle(a, a).print();

			for (const std::string& z : zKeys)
			{
				auto steps = numStepsUntilCycle(a, z);
				steps.print();
				if (steps.isReachable())
					reachable.push_back(std::move(steps));
			}
		}

		/*
		* From analysis it appears that each A only reaches one Z in the data provided.
		* Additionally, the Z is reached exactly on the final instruction,
		* i.e., an integer number of loops around the instruction set is completed when a Z is found.
		* This means the Z is only visited once.
		* 
		* The below code advances each path based on these assumptions:
		* - Each A visits at most only one Z
		* - The step count to reach Z is an integer number of instruction counts.
		* - Therefore we can directly use the step counts to advance the paths.
		* - I.e., we don't need to worry about any instruction offsetting when advancing paths.
		* 
		* Since the data "works out nicely" with these integer multiples, there's probably a nicer way to
		* calculate the final answer - but this runs fast enough (about a minute in Release).
		*/

		std::vector<uint64_t> currentStepCount;
		for (auto& r : reachable)
			currentStepCount.push_back(r.stepCounts.front().second);

		auto allSame = [&currentStepCount] ()
		{
			return std::all_of(
				begin(currentStepCount),
				end(currentStepCount),
				[&currentStepCount] (auto c) { return c == currentStepCount.front(); }
			);
		};

		while (!allSame())
		{
			size_t index = SIZE_MAX;
			size_t minCount = SIZE_MAX;
			for (size_t i = 0; i < currentStepCount.size(); ++i)
			{
				const auto count = currentStepCount[i];
				if (count < minCount)
				{
					minCount = count;
					index = i;
				}
			}

			currentStepCount[index] += reachable[index].stepCounts.front().second;
		}

		return currentStepCount.front();
	}
};

static auto loadData(const char* filename)
{
	std::fstream s{ filename, s.in };
	assert(s.is_open());

	Graph08 graph;

	while (s.peek() != EOF)
	{
		std::string line;
		std::getline(s, line);

		if (graph.instructions.size() == 0)
		{
			for (const char c : line)
				graph.instructions.push_back(c == 'L' ? Instruction08::Left : Instruction08::Right);
		}
		else if (!line.empty())
		{
			std::string key;
			Node08 node;

			for (const auto word : std::views::split(line, ' '))
			{
				const std::string token(&*word.begin(), std::ranges::distance(word));

				if (token == "=")
					continue;
				else if (token.size() == 3)
					key = token;
				else if (token.find('(') != std::string::npos)
					node.left = token.substr(1, 3);
				else if (token.find(')') != std::string::npos)
					node.right = token.substr(0, 3);
			}

			assert(!key.empty());
			assert(!node.left.empty());
			assert(!node.right.empty());

			graph.nodes[key] = node;
		}
	}

	return graph;
}

static uint64_t partOne(const Graph08& graph)
{
	return graph.numStepsPartOne();
}

static uint64_t partTwo(const Graph08& graph)
{
	return graph.partTwoNumSteps();
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

void day08()
{
	processPrintAndAssert("../data/08/test.txt", std::make_pair(2ull, 0ull));
	processPrintAndAssert("../data/08/test2.txt", std::make_pair(6ull, 0ull));
	processPrintAndAssert("../data/08/test3.txt", std::make_pair(0ull, 6ull));
	processPrintAndAssert("../data/08/real.txt", std::make_pair(21389ull, 0ull));
}
