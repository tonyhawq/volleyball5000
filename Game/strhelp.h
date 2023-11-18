#pragma once

#include <string>

namespace strhelp
{
	void findAndRemove(std::string& str, char c)
	{
		str.erase(std::remove(str.begin(), str.end(), c), str.end());
	}
}