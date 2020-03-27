#include "stdafx.h"
#include "Threadpool.h"

Threadpool* Threadpool::ourInstance = nullptr;
void Threadpool::CreateInstance(const unsigned int aMaxTaskCount, const unsigned int aThreadCount)
{
	if (ourInstance == nullptr)
	{
		ourInstance = new Threadpool(aMaxTaskCount, (aThreadCount == 0) ? std::thread::hardware_concurrency() : aThreadCount);
	}
}
Threadpool* Threadpool::GetInstance()
{
	return ourInstance;
}
void Threadpool::DestroyInstance()
{
	if (ourInstance != nullptr)
	{
		delete ourInstance;
		ourInstance = nullptr;
	}
}

Threadpool::Threadpool(const unsigned int aMaxTaskCount, const unsigned int aThreadCount)
{
	myTaskMutex.lock();
	myContinueWork = true;
	myTasks.Init(aMaxTaskCount);
	myWorkingDependancies.Init(aMaxTaskCount);
	myThreads.Init(aThreadCount);
	myThreads.Fill();
	for (unsigned int threadIndex = 0; threadIndex < myThreads.Capacity(); threadIndex++)
	{
		new(&myThreads[threadIndex]) std::thread(&Threadpool::Work, this, threadIndex);
	}
	myWaitingThreads.Init(myThreads.Size());
	myWaitingThreads.Fill();
	for (unsigned int threadIndex = 0; threadIndex < myWaitingThreads.Size(); threadIndex++)
	{
		new(&myWaitingThreads[threadIndex]) std::atomic<bool>(true);
	}
	myTaskMutex.unlock();
}
Threadpool::~Threadpool()
{
	myContinueWork = false;
	while (myThreads.Size() > 0)
	{
		myThreads.GetLast().join();
		myThreads.RemoveLast();
	}
}

long long Threadpool::AddTask(const std::function<void()>& aTask, const bool aSync, const long long aDependancy)
{
	myTaskMutex.lock();
	assert(myTasks.Size() < myTasks.Capacity());
	if (myTasks.Size() >= myTasks.Capacity())
	{
		myTaskMutex.unlock();
		return -1;
	}
	long long id = myNextID;
	myTasks.Add(Task(aTask, id, aDependancy, aSync));
	myNextID++;
	myTaskMutex.unlock();
	for (unsigned int threadIndex = 0; threadIndex < myWaitingThreads.Size(); threadIndex++)
	{
		myWaitingThreads[threadIndex] = false;
	}
	return id;
}

void Threadpool::Work(unsigned int aThreadIndex)
{
	std::function<void()> currentTask;
	while (myContinueWork)
	{
		currentTask = nullptr;
		myTaskMutex.lock();
		if (myTasks.Size() == 0)
		{
			myTaskMutex.unlock();
			myWaitingThreads[aThreadIndex] = true;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			continue;
		}
		else
		{
			long long dependancy = myTasks.GetLast().myDependancy;
			if (dependancy != -1)
			{
				bool dependancyLocked = false;
				myDependancyMutex.lock();
				for (int dependancyIndex = 0; dependancyIndex < myWorkingDependancies.Size(); dependancyIndex++)
				{
					if (myWorkingDependancies[dependancyIndex] == dependancy)
					{
						dependancyLocked = true;
						break;
					}
				}
				myDependancyMutex.unlock();
				if (!dependancyLocked)
				{
					for (unsigned int taskIndex = 0; taskIndex < myTasks.Size() - 1; taskIndex++)
					{
						if (myTasks[taskIndex].myID == dependancy)
						{
							dependancyLocked = true;
							break;
						}
					}
				}
				if (dependancyLocked)
				{
					if (myTasks.Size() > 1)
					{
						Task task = myTasks.GetLast();
						myTasks.RemoveLast();
						myTasks.Insert(0, task);
					}

					myTaskMutex.unlock();
					myWaitingThreads[aThreadIndex] = true;
					std::this_thread::yield();
					continue;
				}
			}
		}
		long long type = myTasks.GetLast().myID;
		if (type != -1)
		{
			myDependancyMutex.lock();
			myWorkingDependancies.Add(type);
			myDependancyMutex.unlock();
		}
		currentTask = myTasks.GetLast().myFunction;
		myWaitingThreads[aThreadIndex] = !myTasks.GetLast().mySync;
		myTasks.RemoveLast();
		myTaskMutex.unlock();

		currentTask();
		if (type != -1)
		{
			myDependancyMutex.lock();
			myWorkingDependancies.RemoveCyclic(type);
			myDependancyMutex.unlock();
		}
	}
}

void Threadpool::WaitForTask(const long long aID)
{
	bool ready = false;
	do
	{
		ready = true;
		myTaskMutex.lock();
		for (unsigned int taskIndex = 0; taskIndex < myTasks.Size(); taskIndex++)
		{
			if (myTasks[taskIndex].myID == aID)
			{
				ready = false;
				break;
			}
		}
		myTaskMutex.unlock();
		if (ready)
		{
			myDependancyMutex.lock();
			for (int dependancyIndex = 0; dependancyIndex < myWorkingDependancies.Size(); dependancyIndex++)
			{
				if (myWorkingDependancies[dependancyIndex] == aID)
				{
					ready = false;
					break;
				}
			}
			myDependancyMutex.unlock();
		}
		if (!ready)
		{
			std::this_thread::yield();
		}
	} while (!ready);
}

void Threadpool::Sync()
{
	bool ready = false;
	do
	{
		ready = true;
		for (unsigned int threadIndex = 0; threadIndex < myThreads.Size(); threadIndex++)
		{
			if (!myWaitingThreads[threadIndex])
			{
				ready = false;
				break;
			}
		}
		if (!ready)
		{
			std::this_thread::yield();
		}
	} while (!ready);
}
