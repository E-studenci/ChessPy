#pragma once 
#include <chrono>


#if defined(_WIN32) || defined(WIN32) 
#define OS_Windows
#endif

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
#ifdef OS_Windows
	std::chrono::steady_clock::time_point start;
#else
	std::chrono::system_clock::time_point start;
#endif
	std::chrono::milliseconds allocatedTime;
};