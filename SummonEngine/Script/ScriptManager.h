#pragma once
#include "..\Squirrel\Include\squirrel.h"
#include "ScriptStructs.h"
#include "SafeCast.h"
#include <HashMap.hpp>
#include <GrowingArray.hpp>
#include <iostream>
#include <functional>
#include <vector>
#include <string>
#include <any>

class NodeInstance;

class ScriptManager
{
public:
	static void CreateInstance();
	static ScriptManager* GetInstance();
	static void DestroyInstance();

	void OpenFile(const std::string& aFile, const std::string& aClass = std::string());
	void CloseFile(const std::string& aFile);
	void RegisterScript(const std::string& aFile);
	void RegisterFunction(const std::function<long long(const std::vector<std::any>&, std::vector<std::any>&)>& aFunction, const std::string& aFunctionName);
	void RegisterFunction(const std::function<long long(const std::vector<std::any>&, std::vector<std::any>&)>& aFunction, const std::string& aFunctionName, const std::string& aScript);
	void RegisterEventListener(const int aEvent, const std::string& aFunctionName, const std::string& aScript);
	void RegisterEventListener(const int aEvent, const std::string& aFunctionName, NodeInstance* aNode);
	void UnregisterEventListener(const int aEvent, const std::string& aFunctionName, const std::string& aScript);
	void UnregisterEventListener(const int aEvent, const std::string& aFunctionName, NodeInstance* aNode);
	void CallFunction(const std::string& aFunctionName);
	void CallFunction(const std::string& aFunctionName, const std::vector<std::any>& aArguments);
	std::vector<std::any> CallFunction(const std::string& aFunctionName, const std::string& aScript);
	std::vector<std::any> CallFunction(const std::string& aFunctionName, const std::vector<std::any>& aArguments, const std::string& aScript, const std::string& aClass = std::string());
	void CallEvent(const int aEvent);
	void SetErrorString(const std::string& aString);

private:
	static SQInteger SquirrelFunction(HSQUIRRELVM aVM);

	ScriptManager();
	~ScriptManager();

	void PrintError();
	void AddFunctionToScript(const std::string& aFunction, HSQUIRRELVM& aScriptVM);
	long long RegisterEventListenerSquirrel(const std::vector<std::any>& aArumentList, std::vector<std::any>& aOutReturnList);
	long long UnregisterEventListenerSquirrel(const std::vector<std::any>& aArumentList, std::vector<std::any>& aOutReturnList);

private:
	static ScriptManager* ourInstance;

	CU::HashMap<std::string, ScriptFile> myScripts;
	//global functions
	CU::HashMap<std::string, ScriptFunction> myFunctions;
	//local functions, names are global
	CU::HashMap<std::string, ScriptFunction> mySpecialFunctions;
	//true local functions
	CU::HashMap<std::string, std::vector<ScriptFunction>> myScriptFunctions;
	std::vector<std::vector<EventFunction>> myEvents;
	std::vector<EventFunction> myEventsToAdd;
	std::vector<EventFunction> myEventsToRemove;
	std::vector<std::string> myRunningScriptStack;
	std::string myErrorString;
};