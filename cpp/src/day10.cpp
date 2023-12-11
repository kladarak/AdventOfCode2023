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

namespace d10
{
	struct Pos
	{
		int x = -1;
		int y = -1;

		bool isValid() const { return x != -1 && y != -1; }

		auto operator<=>(const Pos&) const = default;

		Pos left()  const { assert(isValid()); return { x - 1, y }; }
		Pos right() const { assert(isValid()); return { x + 1, y }; }
		Pos above() const { assert(isValid()); return { x, y - 1 }; }
		Pos below() const { assert(isValid()); return { x, y + 1 }; }
	};

	struct Pipe
	{
		Pos pos;
		Pos neighbour1;
		Pos neighbour2;

		bool isValid() const { return pos.isValid(); }
		
		bool isConnected() const
		{
			return neighbour1.isValid()
				&& neighbour2.isValid(); 
		}

		bool isConnectedTo(const Pos& p) const
		{
			return neighbour1 == p
				|| neighbour2 == p;
		}

		Pos getNextNeighbour(const Pos& prev) const
		{
			assert(neighbour1 == prev || neighbour2 == prev);
			return neighbour1 == prev ? neighbour2 : neighbour1;
		}

		Pos left()  const { return pos.left(); }
		Pos right() const { return pos.right(); }
		Pos above() const { return pos.above(); }
		Pos below() const { return pos.below(); }
	};

	struct Graph
	{
		std::vector<std::vector<Pipe>> pipes;
		Pos start;

		bool isValidPos(const Pos& pos) const
		{
			return pos.isValid()
				&& pos.y < (int) pipes.size()
				&& pos.x < (int) pipes.front().size();
		}

		const Pipe& getPipe(const Pos& pos) const
		{
			assert(isValidPos(pos));
			return pipes[pos.y][pos.x];
		}

		Pipe& getPipe(const Pos& pos)
		{
			assert(isValidPos(pos));
			return pipes[pos.y][pos.x];
		}

		void removeJunkPipes()
		{
			Pos prevPos = start;
			Pos pos = getPipe(prevPos).neighbour1;

			std::set<Pos> mainLoop;

			while (mainLoop.insert(pos).second)
			{
				const Pipe& pipe = getPipe(pos);
				const Pos nextPos = pipe.getNextNeighbour(prevPos);
				prevPos = pos;
				pos = nextPos;
			}

			for (auto& row : pipes)
			{
				for (Pipe& pipe : row)
				{
					if (pipe.isValid() && !mainLoop.contains(pipe.pos))
						pipe = Pipe();
				}
			}
		}

		bool isInsidePipeLoop(const Pos& pos) const
		{
			// Raycast left and count how many pipe crossings there are.
			// An odd number of crossings means we're inside the loop.
			int crossings = 0;

			Pos leftPipePos = pos.left();

			while (leftPipePos.isValid())
			{
				const Pipe* leftPipe = &getPipe(leftPipePos);
				if (!leftPipe->isValid())
				{
					// keep advancing left until hitting a pipe
					leftPipePos = leftPipePos.left();
					continue;
				}

				// For running segments of pipe, e.g., L-J or FJ etc,
				// we need to test the ends to see if they go the same direction or not.
				// If they're the same direction then the pipe was not crossed
				// (i.e., we're still on the same side of the loop)
				// e.g., LJ does not cross but FJ does.
				const bool goesDown = leftPipe->isConnectedTo(leftPipe->below());

				const Pipe* lastPipe = leftPipe;
				while (lastPipe->isConnectedTo(lastPipe->left()))
					lastPipe = &getPipe(lastPipe->left());

				if (lastPipe == leftPipe)
					crossings++; // Pipe was a vertical pipe
				else if (lastPipe->isConnectedTo(lastPipe->below()) != goesDown)
					crossings++; // Pipe end goes in a different direction than the start

				leftPipePos = lastPipe->left();
			}

			return (crossings % 2) == 1;
		}
	};

	auto loadData(const char* filename)
	{
		std::fstream s{ filename, s.in };
		assert(s.is_open());

		Graph graph;

		while (s.peek() != EOF)
		{
			std::string line;
			std::getline(s, line);
			assert(line.size() > 0);

			const int row = (int) graph.pipes.size();

			graph.pipes.push_back({});
			std::vector<Pipe>& pipes = graph.pipes.back();
			pipes.resize(line.size());

			for (int col = 0; col < (int) line.size(); ++col)
			{
				Pipe& pipe = pipes[col];
				pipe.pos = { col, row };

				switch (line[col])
				{
					case '.': 
						break;

					case 'S':
						graph.start = pipe.pos;
						break;

					case '-':
						pipe.neighbour1 = pipe.left();
						pipe.neighbour2 = pipe.right();
						break;

					case '|':
						pipe.neighbour1 = pipe.above();
						pipe.neighbour2 = pipe.below();
						break;

					case '7':
						pipe.neighbour1 = pipe.left();
						pipe.neighbour2 = pipe.below();
						break;

					case 'J':
						pipe.neighbour1 = pipe.left();
						pipe.neighbour2 = pipe.above();
						break;

					case 'L':
						pipe.neighbour1 = pipe.above();
						pipe.neighbour2 = pipe.right();
						break;

					case 'F':
						pipe.neighbour1 = pipe.below();
						pipe.neighbour2 = pipe.right();
						break;

					default:
						assert(false);
						break;
				}
			}
		}

		assert(graph.start.isValid());

		Pipe& startPipe = graph.getPipe(graph.start);

		auto setStartNeighbourIfConnected = [&graph, &startPipe] (const Pos& pos)
		{
			if (graph.isValidPos(pos) && graph.getPipe(pos).isConnectedTo(graph.start))
				(startPipe.neighbour1.isValid() ? startPipe.neighbour2 : startPipe.neighbour1) = pos;
		};

		setStartNeighbourIfConnected(startPipe.left());
		setStartNeighbourIfConnected(startPipe.right());
		setStartNeighbourIfConnected(startPipe.above());
		setStartNeighbourIfConnected(startPipe.below());

		assert(startPipe.isConnected());

		graph.removeJunkPipes();

		return graph;
	}
}

static uint64_t partOne(const d10::Graph& graph)
{
	using namespace d10;

	Pos prevPos = graph.start;
	Pos pos = graph.getPipe(prevPos).neighbour1;
	uint64_t stepsToLoop = 1;

	while (pos != graph.start)
	{
		const Pipe& pipe = graph.getPipe(pos);
		const Pos nextPos = pipe.getNextNeighbour(prevPos);
		prevPos = pos;
		pos = nextPos;
		++stepsToLoop;
	}

	assert(stepsToLoop % 2 == 0);
	return stepsToLoop / 2;
}

static uint64_t partTwo(const d10::Graph& graph)
{
	using namespace d10;

	uint64_t numInside = 0;

	for (int row = 0; row < (int) graph.pipes.size(); ++row)
	{
		bool wasPipe = false;
		bool inside = false;

		for (int col = 0; col < (int) graph.pipes.front().size(); ++col)
		{
			const Pos pos = { col, row };
			const Pipe& pipe = graph.getPipe(pos);
			if (pipe.isValid())
			{
				wasPipe = true;
				continue;
			}

			// boundary change - test if region is inside or outside the loop, and cache while we count this row.
			if (wasPipe)
				inside = graph.isInsidePipeLoop(pos);

			numInside += inside;
			wasPipe = pipe.isValid();
		}
	}

	return numInside;
}

static std::pair<uint64_t, uint64_t> process(const char* filename)
{
	const auto data = d10::loadData(filename);
	return std::make_pair(partOne(data), partTwo(data));
}

static void processPrintAndAssert(const char* filename, std::optional<std::pair<uint64_t, uint64_t>> expected = {})
{
	const auto result = process(filename);
	std::cout << "Part 1: " << result.first << " Part 2: " << result.second << std::endl;

	assert(!expected || expected.value().first == 0 || result.first == expected.value().first);
	assert(!expected || expected.value().second == 0 || result.second == expected.value().second);
}

void day10()
{
	processPrintAndAssert("../data/10/test.txt", std::make_pair(4ull, 1ull));
	processPrintAndAssert("../data/10/test2.txt", std::make_pair(8ull, 1ull));
	processPrintAndAssert("../data/10/test3.txt", std::make_pair(0ull, 4ull));
	processPrintAndAssert("../data/10/test4.txt", std::make_pair(0ull, 4ull));
	processPrintAndAssert("../data/10/test5.txt", std::make_pair(0ull, 8ull));
	processPrintAndAssert("../data/10/test6.txt", std::make_pair(0ull, 10ull));
	processPrintAndAssert("../data/10/real.txt", std::make_pair(6773ull, 493ull));
}
