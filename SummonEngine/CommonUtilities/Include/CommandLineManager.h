#pragma once
#include "GrowingArray.hpp"
#include <string>
#include <vector>

namespace CU
{
	struct CommandLineParameter
	{
		std::wstring Parameter;
		std::vector<std::wstring> Argument;
	};

	class CommandLineManager
	{
	public:
		static void CreateInstance();
		static void DestroyInstance();
		static CommandLineManager* GetInstance();

		bool HasParameter(const char* aParameter);
		bool HasArgument(const char* aParameter, const char* aArgument);
		std::string GetArgument(const char* aParameter, unsigned int aArgumentIndex);

	private:
		CommandLineManager();
		~CommandLineManager() = default;

		static CommandLineManager* ourInstance;

		std::vector<CommandLineParameter> myParameters;
	};
}