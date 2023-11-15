#include "Timer.h"

void vbl::Timer::set()
{
	this->startTime = std::chrono::high_resolution_clock::now();
}

float vbl::Timer::time()
{
	std::chrono::duration<float, std::milli> tempValue = std::chrono::high_resolution_clock::now() - startTime;
	return tempValue.count();
}
