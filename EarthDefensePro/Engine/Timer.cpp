#include "Timer.h"

Timer::Timer()
{
}

Timer::~Timer()
{
}

void Timer::Start()
{
    start = std::chrono::high_resolution_clock::now();
}

double Timer::GetDeltaTime()
{
    auto end = std::chrono::high_resolution_clock::now();
    auto deltaTime = std::chrono::duration<double, std::milli>(end - start);
    start = end; // Update start time for the next measurement
    return deltaTime.count();
}