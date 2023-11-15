#pragma once

#include <string>

#include "maf.h"

namespace loadutils
{
	inline maf::fvec2 loadVec2(const std::string& str, size_t* startAt = NULL)
	{
		size_t i = 0;
		size_t f = 0;
		int current = 0;
		maf::fvec2 res{};
		if (startAt)
		{
			i = *startAt;
		}
		bool run = true;
		while (run)
		{
			current++;
			if (current >= 3)
			{
				break;
			}
			f = str.find(',', i);
			if (f == std::string::npos)
			{
				run = false;
				f = str.length();
			}
			std::string valueStr = str.substr(i, f - i);
			if (current == 1)
			{
				res.x = std::stof(valueStr);
			}
			else
			{
				res.y = std::stof(valueStr);
			}
			i = f + 1;
		}
		if (startAt)
		{
			*startAt = i;
		}
		return res;
	}
}