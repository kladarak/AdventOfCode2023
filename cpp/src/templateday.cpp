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

void day_X_()
{
	assert(process("../data/_X_/test.txt") == 0);
	assert(process("../data/_X_/real.txt") == 0);
	assert(process("../data/_X_/test2.txt") == 0);
	assert(process("../data/_X_/real.txt") == 0);
}
