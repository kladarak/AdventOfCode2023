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

struct Data05
{
	struct RemapRangeResult
	{
		std::optional<std::pair<uint64_t, uint64_t>> remapped;
		std::vector<std::pair<uint64_t, uint64_t>> unmapped;
	};

	struct Mapping
	{
		uint64_t dst = 0;
		uint64_t src = 0;
		uint64_t rng = 0;

		std::optional<uint64_t> remap(uint64_t value) const
		{
			if (src <= value && value <= src + rng - 1)
				return dst + (value - src);
			else
				return std::optional<uint64_t>();
		}

		bool overlaps(const std::pair<uint64_t, uint64_t>& range) const
		{
			return range.first <= (src + rng - 1)
				&& range.first + range.second - 1 >= src;
		}

		RemapRangeResult remapRange(const std::pair<uint64_t, uint64_t>& range) const
		{
			RemapRangeResult result;

			if (overlaps(range))
			{
				const uint64_t first = std::max(range.first, src);
				const uint64_t last = std::min(range.first + range.second - 1, src + rng - 1);

				const uint64_t firstRemapped = remap(first).value();
				const uint64_t lastRemapped = remap(last).value();
				result.remapped = std::make_pair(firstRemapped, lastRemapped - firstRemapped + 1);

				if (range.first < first)
					result.unmapped.emplace_back(range.first, first - range.first);

				if (last < range.first + range.second - 1)
					result.unmapped.emplace_back(last + 1, range.first + range.second - 1 - last);
			}
			else
			{
				result.unmapped.push_back(range);
			}

			return result;
		}
	};

	std::vector<uint64_t> seeds;
	std::vector<std::pair<uint64_t, uint64_t>> seedRanges; // first, range
	std::vector<std::vector<Mapping>> mappings;

	uint64_t resolveLocation(uint64_t seed) const
	{
		uint64_t location = seed;

		for (const std::vector<Mapping>& mappingList : mappings)
		{
			for (const Mapping& mapping : mappingList)
			{
				if (auto remapped = mapping.remap(location))
				{
					location = remapped.value();
					break;
				}
			}
		}

		return location;
	}

	std::vector<std::pair<uint64_t, uint64_t>> evaluateSeedRangeLocations() const
	{
		std::vector<std::pair<uint64_t, uint64_t>> locations = seedRanges;

		for (const std::vector<Mapping>& mappingList : mappings)
		{
			std::vector<std::pair<uint64_t, uint64_t>> toProcess = locations;
			locations.clear();

			for (const Mapping& mapping : mappingList)
			{
				for (int i = 0; i < toProcess.size(); ++i)
				{
					const std::pair<uint64_t, uint64_t> range = toProcess[i];
					const RemapRangeResult result = mapping.remapRange(range);

					if (result.remapped)
					{
						locations.push_back(result.remapped.value());
						toProcess.erase(toProcess.begin() + i);
						toProcess.insert(toProcess.end(), begin(result.unmapped), end(result.unmapped));
						--i;
					}
				}
			}

			locations.insert(locations.end(), begin(toProcess), end(toProcess));
		}

		return locations;
	}
};

static auto loadData(const char* filename)
{
	std::fstream s{ filename, s.in };
	assert(s.is_open());

	Data05 data;

	while (s.peek() != EOF)
	{
		std::string line;
		std::getline(s, line);

		if (data.seeds.size() == 0)
		{
			size_t index = 0;

			for (const auto word : std::views::split(line, ' '))
			{
				const std::string token(&*word.begin(), std::ranges::distance(word));

				if (token != "seeds:")
				{
					const uint64_t seed = std::stoull(token);
					data.seeds.push_back(seed);

					if (index % 2 == 0)
						data.seedRanges.emplace_back(seed, 0);
					else
						data.seedRanges.back().second = seed;

					++index;
				}
			}
		}
		else if (line.size() == 0)
		{
			data.mappings.push_back({});
		}
		else if (std::isdigit(line.front()))
		{
			data.mappings.back().push_back({});

			size_t index = 0;

			for (const auto word : std::views::split(line, ' '))
			{
				const std::string token(&*word.begin(), std::ranges::distance(word));

				switch (index)
				{
					case 0: data.mappings.back().back().dst = std::stoull(token); break;
					case 1: data.mappings.back().back().src = std::stoull(token); break;
					case 2: data.mappings.back().back().rng = std::stoull(token); break;
				}

				++index;
			}
		}
	}

	return data;
}

static uint64_t partOne(const auto& data)
{
	return std::accumulate(
		begin(data.seeds),
		end(data.seeds),
		UINT64_MAX,
		[&data] (uint64_t loc, const uint64_t& seed) { return std::min(loc, data.resolveLocation(seed)); }
	);
}

static uint64_t partTwo(const auto& data)
{
	const std::vector<std::pair<uint64_t, uint64_t>> locations = data.evaluateSeedRangeLocations();

	return std::accumulate(
		begin(locations),
		end(locations),
		UINT64_MAX,
		[&data] (uint64_t loc, const std::pair<uint64_t, uint64_t>& locationRange) { return std::min(loc, locationRange.first); }
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

void day05()
{
	processPrintAndAssert("../data/05/test.txt", std::make_pair(35ull, 46ull));
	processPrintAndAssert("../data/05/real.txt", std::make_pair(579439039ull, 7873084ull));
}
