#pragma once
#include "GrowingArray.hpp"
#include <filesystem>
#include <functional>
#include <string>

namespace CU
{
	class FileWatcher
	{
	public:
		static void CreateInstance();
		static FileWatcher* GetInstance();
		static void DestroyInstance();

		void AddCallback(const std::string& aFilePath, const std::function<void()>& aChangedFunction, const std::function<void()>& aLostFunction = nullptr);
		void Update();

	private:
		struct Callback
		{
			std::filesystem::file_time_type myPreviousTime;
			std::function<void()> myChangedFunction;
			std::function<void()> myLostFunction;
			std::string myPath;
			bool myFoundFile = false;
		};
	private:
		FileWatcher();
		~FileWatcher();

		static FileWatcher* ourInstance;
		CU::GrowingArray<Callback> myCallbackList;
	};
}