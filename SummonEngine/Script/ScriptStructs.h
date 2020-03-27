#pragma once
#include "..\Squirrel\Include\squirrel.h"
#include <GrowingArray.hpp>
#include <functional>
#include <vector>
#include <string>
#include <any>

class NodeInstance;

struct ScriptFunction
{
	ScriptFunction()
	{
	}
	ScriptFunction(const std::function<long long(const std::vector<std::any>&, std::vector<std::any>&)>& aFunction)
	{
		myFunction = aFunction;
	}
	ScriptFunction(const std::function<long long(const std::vector<std::any>&, std::vector<std::any>&)>& aFunction, const std::string& aName)
	{
		myFunction = aFunction;
		myName = aName;
	}
	std::function<long long(const std::vector<std::any>&, std::vector<std::any>&)> myFunction;
	std::string myName;
};
struct ScriptFile//needs path to work fully with running script stack
{
	ScriptFile()
	{
		mySquirrelVM = nullptr;
		myFileOpen = false;
	}
	~ScriptFile()
	{
		myFileOpen = false;
		if (mySquirrelVM != nullptr)
		{
			sq_close(mySquirrelVM);
		}
	}
	std::vector<std::string> myFunctions;
	HSQUIRRELVM mySquirrelVM;
	bool myFileOpen;
};
struct EventFunction
{
	EventFunction()
	{
		myNode = nullptr;
		myEvent = -1;
	}
	EventFunction(const std::string& aFunctionName, const std::string& aScript, const int aEventID)
	{
		myFunction = aFunctionName;
		myScript = aScript;
		myEvent = aEventID;
		myNode = nullptr;
	}
	EventFunction(const std::string& aFunctionName, NodeInstance* aNode, const int aEventID)
	{
		myFunction = aFunctionName;
		myNode = aNode;
		myEvent = aEventID;
	}
	EventFunction& operator=(const EventFunction& aEventFunction)
	{
		myFunction = aEventFunction.myFunction;
		myScript = aEventFunction.myScript;
		myNode = aEventFunction.myNode;
		myEvent = aEventFunction.myEvent;
		return *this;
	}
	std::string myFunction;
	std::string myScript;
	NodeInstance* myNode;
	int myEvent;
};