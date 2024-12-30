#pragma once
#include <cstdlib>

namespace maf
{
	inline int random(int min, int max)
	{
		if (min > max)
		{
			return 0;
		}
		return (rand() % (abs(max) + abs(min) + 1)) + min;
	}
	inline float random(float min, float max)
	{
		return (float)random(int(min * 100), int(max * 100)) / 100.0f;
	}

	// works with all std containers except forward_list
	template <class C, class T>
	T randomItem(C from)
	{
		return from[maf::random(0, (int)(from.size() - 1))];
	};
}
