#include "Timer.h"

Timer::Timer()
{
	QueryPerformanceFrequency(&frequency);
}

void Timer::Start()
{
	QueryPerformanceCounter(&start);
}

double Timer::Stop()
{
	LARGE_INTEGER stop;
	QueryPerformanceCounter(&stop);

	// Get the elapsed time in ticks.
	LARGE_INTEGER elapsed;
	elapsed.QuadPart = stop.QuadPart - start.QuadPart;

	// Multiply by million. Guards against precision loss.
	elapsed.QuadPart *= 1000000;

	// Convert ticks to microseconds.
	elapsed.QuadPart /= frequency.QuadPart;

	return double(elapsed.QuadPart) / 1000.0;
}