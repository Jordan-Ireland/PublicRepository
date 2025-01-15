#pragma once
#include <chrono>

class Timer
{
private:
    std::chrono::high_resolution_clock::time_point start;

public:
    Timer();
    ~Timer();

    void Start();
    double GetDeltaTime();
};