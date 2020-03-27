#pragma once
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <GrowingArray.hpp>
#include <array>

class Threadpool
{
public:
	static void CreateInstance(const unsigned int aMaxTaskCount, const unsigned int aThreadCount = 0);
	static Threadpool* GetInstance();
	static void DestroyInstance();

	Threadpool(const unsigned int aMaxTaskCount, const unsigned int aThreadCount);
	~Threadpool();

	long long AddTask(const std::function<void()>& aTask, const bool aSync = true, const long long aDependancy = -1);
	void Work(unsigned int aThreadIndex);
	void WaitForTask(const long long aID);
	void Sync();

private:
	struct Task
	{
		Task()
		{
			myID = -1;
			myDependancy = -1;
			mySync = false;
		}
		Task(const std::function<void()>& aFunction, const long long aID, const long long aDependancy, const bool aSync)
		{
			myFunction = aFunction;
			myID = aID;
			myDependancy = aDependancy;
			mySync = aSync;
		}
		std::function<void()> myFunction;
		long long myID;
		long long myDependancy;
		bool mySync;
	};
private:
	static Threadpool* ourInstance;

	CU::GrowingArray<std::thread, unsigned int> myThreads;
	CU::GrowingArray<Task, unsigned int> myTasks;
	CU::GrowingArray<std::atomic<bool>, unsigned int> myWaitingThreads;
	CU::GrowingArray<long long, int> myWorkingDependancies;
	std::atomic<long long> myNextID;
	std::atomic<bool> myContinueWork;
	std::mutex myTaskMutex;
	std::mutex myDependancyMutex;
};