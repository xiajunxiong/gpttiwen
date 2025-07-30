#include "unittest/catch.hpp"
#include "servercommon/servercommon.h"


TEST_CASE("Test RandomNum Function", "[RandNum(int min, int max)]") {
	for (int i = 0; i < 10000; ++i)
	{
		int x = RandomNum(MAX_INT);

		CHECK(RandomNum(x, x) == 0);
		CHECK(RandomNum(x, x + 1) == x);
		CHECK(RandomNum(x + 1, x) == x);
	}
}
