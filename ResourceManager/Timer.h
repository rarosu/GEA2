#pragma once

#include <Windows.h>

class Timer
{
public:
	Timer();

	// Restarts the timer.
	void Start();

	// Stops the timer and returns the time measured in seconds.
	double Stop();
private:
	LARGE_INTEGER frequency;
	LARGE_INTEGER start;
};

