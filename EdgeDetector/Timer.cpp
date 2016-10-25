#include "Timer.h"

Timer::Timer()
{
}

Timer::~Timer()
{
}

void Timer::start()
{
	is_stopped = false;
	begin = high_resolution_clock::now();
}

void Timer::stop()
{
	is_stopped = true;
	end = high_resolution_clock::now();
}

double Timer::get_elapsed_time()
{
	high_resolution_clock::time_point end_time;

	if (!is_stopped)
	{
		end_time = high_resolution_clock::now();
	}
	else
	{
		end_time = end;
	}

	return std::chrono::duration_cast<std::chrono::milliseconds>(end_time - begin).count() / 1000.0;
}
