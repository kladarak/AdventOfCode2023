#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

static auto process(const char* filename)
{
	std::fstream s{ filename, s.in };
	assert(s.is_open());

	return 0;
}

void day02()
{
	assert(process("../data/02/test.txt") == 0);
	assert(process("../data/02/real.txt") == 0);
	assert(process("../data/02/test2.txt") == 0);
	assert(process("../data/02/real.txt") == 0);
}
