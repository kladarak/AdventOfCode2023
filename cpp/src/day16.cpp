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

namespace d16
{
	struct Position
	{
		int x = -1;
		int y = -1;

		bool isValid() const { return x != -1 && y != -1; }

		auto operator<=>(const Position&) const = default;

		Position left()		const { assert(isValid()); return { x - 1, y }; }
		Position right()	const { assert(isValid()); return { x + 1, y }; }
		Position up()		const { assert(isValid()); return { x, y - 1 }; }
		Position down()		const { assert(isValid()); return { x, y + 1 }; }
	};

	enum class Direction
	{
		Left,
		Right,
		Up,
		Down
	};

	struct Ray
	{
		Position pos;
		Direction dir = Direction::Left;

		auto operator<=>(const Ray&) const = default;

		Ray goLeft() const	{ return { pos.left(), Direction::Left }; }
		Ray goRight() const { return { pos.right(), Direction::Right}; }
		Ray goUp() const	{ return { pos.up(), Direction::Up }; }
		Ray goDown() const	{ return { pos.down(), Direction::Down}; }
	};

	struct Grid
	{
		std::vector<std::string> rows;

		explicit Grid(const char* filename)
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
		}

		uint64_t traverseAndCountVisited(const Ray& start) const
		{
			std::set<Ray> visitedRays;
			std::set<Position> visitedPositions;

			std::vector<Ray> queue;
			queue.push_back(start);

			while (!queue.empty())
			{
				Ray ray = queue.back();
				queue.pop_back();

				if (!visitedRays.insert(ray).second)
					continue;

				visitedPositions.insert(ray.pos);

				const std::pair<Ray, Ray> next = advance(ray);

				if (isValidPos(next.first.pos))
					queue.push_back(next.first);

				if (isValidPos(next.second.pos))
					queue.push_back(next.second);
			}

			return visitedPositions.size();
		}

		bool isValidPos(const Position& pos) const
		{
			return pos.isValid()
				&& pos.x < (int) rows.front().size()
				&& pos.y < (int) rows.size();
		}

		std::pair<Ray, Ray> advance(const Ray& ray) const
		{
			const char c = rows[ray.pos.y][ray.pos.x];

			switch (c)
			{
				case '.':
				{
					switch (ray.dir)
					{
						case Direction::Up:		return { ray.goUp(), {} };
						case Direction::Down:	return { ray.goDown(), {} };
						case Direction::Left:	return { ray.goLeft(), {} };
						case Direction::Right:	return { ray.goRight(), {} };
					}
				}

				case '/':
				{
					switch (ray.dir)
					{
						case Direction::Up:		return { ray.goRight(), {} };
						case Direction::Down:	return { ray.goLeft(), {} };
						case Direction::Left:	return { ray.goDown(), {} };
						case Direction::Right:	return { ray.goUp(), {} };
					}
				}

				case '\\':
				{
					switch (ray.dir)
					{
						case Direction::Up:		return { ray.goLeft(), {} };
						case Direction::Down:	return { ray.goRight(), {} };
						case Direction::Left:	return { ray.goUp(), {} };
						case Direction::Right:	return { ray.goDown(), {} };
					}
				}

				case '|':
				{
					switch (ray.dir)
					{
						case Direction::Up:		return { ray.goUp(), {} };
						case Direction::Down:	return { ray.goDown(), {} };
						case Direction::Left:	return { ray.goUp(), ray.goDown() }; 
						case Direction::Right:	return { ray.goUp(), ray.goDown() };
					}
				}

				case '-':
				{
					switch (ray.dir)
					{
						case Direction::Up:		return { ray.goLeft(), ray.goRight() };
						case Direction::Down:	return { ray.goLeft(), ray.goRight() };
						case Direction::Left:	return { ray.goLeft(), {} }; 
						case Direction::Right:	return { ray.goRight(), {} };
					}
				}
			}

			return {};
		}
	};

	static uint64_t partOne(const Grid& grid)
	{
		return grid.traverseAndCountVisited({{0, 0}, Direction::Right});
	}

	static uint64_t partTwo(const Grid& grid)
	{
		// Brute forced, but fast enough.

		uint64_t mostVisited = 0;
		
		for (size_t x = 0; x < grid.rows.front().size(); ++x)
			mostVisited = std::max(mostVisited, grid.traverseAndCountVisited({ {(int) x, 0}, Direction::Down }));

		for (size_t x = 0; x < grid.rows.front().size(); ++x)
			mostVisited = std::max(mostVisited, grid.traverseAndCountVisited({ {(int) x, (int) grid.rows.size() - 1 }, Direction::Up }));

		for (size_t y = 0; y < grid.rows.size(); ++y)
			mostVisited = std::max(mostVisited, grid.traverseAndCountVisited({ {0, (int) y}, Direction::Right }));

		for (size_t y = 0; y < grid.rows.size(); ++y)
			mostVisited = std::max(mostVisited, grid.traverseAndCountVisited({ {(int) grid.rows.front().size() - 1, (int) y}, Direction::Left }));

		return mostVisited;
	}

	static std::pair<uint64_t, uint64_t> process(const char* filename)
	{
		const Grid grid(filename);
		return std::make_pair(partOne(grid), partTwo(grid));
	}

	static void processPrintAndAssert(const char* filename, std::pair<uint64_t, uint64_t> expected)
	{
		const auto result = process(filename);
		std::cout << "Part 1: " << result.first << " Part 2: " << result.second << std::endl;

		assert(expected.first == 0 || result.first == expected.first);
		assert(expected.second == 0 || result.second == expected.second);
	}
}

void day16()
{
	d16::processPrintAndAssert("../data/16/test.txt", std::make_pair(46ull, 51ull));
	d16::processPrintAndAssert("../data/16/real.txt", std::make_pair(7060ull, 7493ull));
}
