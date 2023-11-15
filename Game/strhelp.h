#pragma once

#include <string>

namespace strhelp
{
	void findAndRemove(std::string& str, char c)
	{
		printf("removing %d from\n%s\n", c, str.c_str());
		str.erase(std::remove(str.begin(), str.end(), c), str.end());
		printf("\n");
	}
}