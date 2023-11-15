#pragma once

#include <chrono>

namespace vbl
{
	class Timer
	{
	public:
		void set();
		float time();
	private:
		std::chrono::high_resolution_clock::time_point startTime;
	};
}

