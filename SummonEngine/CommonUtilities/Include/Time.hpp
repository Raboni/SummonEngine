#pragma once
#include <chrono>
#include <string>

namespace CU
{
	class Time
	{
	public:
		static long long GetCurrentTimeStamp();

		Time();
		Time(const Time &aTime) = delete;
		Time& operator=(const Time &aTime) = delete;
		void Update();
		float GetDeltaTime() const;
		float GetTotalTime() const;
		std::string GetCurrentTime();

		static void StartStopWatch();
		static float EndStopWatch();
	private:
		static std::chrono::high_resolution_clock::time_point ourStopWatchStart;
		std::chrono::high_resolution_clock::time_point myFirstTimePoint;
		std::chrono::high_resolution_clock::time_point myLastTimePoint;
		std::chrono::high_resolution_clock::time_point myCurrentTimePoint;
	};
}