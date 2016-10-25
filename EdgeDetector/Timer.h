#pragma once

#include <chrono>

using std::chrono::high_resolution_clock;

class Timer
{
private:
	bool is_stopped;
	high_resolution_clock::time_point begin;
	high_resolution_clock::time_point end;
public:
	Timer();
	~Timer();
	void start();
	void stop();
	double get_elapsed_time();
};
