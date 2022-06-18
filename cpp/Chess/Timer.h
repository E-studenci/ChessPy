#pragma once 
#include <chrono>

class Timer {
public:
	Timer(long allocatedTimeInMillis) {
		this->allocatedTime = std::chrono::milliseconds(allocatedTimeInMillis);
		this->start = std::chrono::high_resolution_clock::now();
	}
	Timer() : Timer(10000) {};
	void Start(long allocatedTimeInMillis) {
		this->allocatedTime = std::chrono::milliseconds(allocatedTimeInMillis);
		this->start = std::chrono::high_resolution_clock::now();
	}
	bool Poll(int nodes) {
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start) > allocatedTime;
	}
private:
	std::chrono::system_clock::time_point start;
	std::chrono::milliseconds allocatedTime;
};