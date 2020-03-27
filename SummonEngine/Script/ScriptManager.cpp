#include "ScriptManager.h"
#include "NodeInstance.h"
#include "sqstdstream.h"
#include "sqstdaux.h"
#include "sqstdblob.h"
#include "sqstdio.h"
#include "sqstdsystem.h"
#include "sqstdmath.h"
#include "sqstdstring.h"
#include <Time.hpp>
#include <Levenshtein.h>
#include <FileWatcher.h>
#include <DebugTools.h>
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <cstdarg>
#include <typeindex>

void Print(HSQUIRRELVM aVM, const SQChar* aString, ...)
{
	char string[4096];
	va_list args;
	va_start(args, aString);
	vsprintf_s(string, aString, args);
	std::cout << string << std::endl;
	va_end(args);
}
void ErrorPrint(HSQUIRRELVM aVM, const SQChar* aString, ...)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);

	const SQInteger top = sq_gettop(aVM);
	const char* str = nullptr;
	sq_getstring(aVM, top, &str);
	if (str != nullptr)
	{
		ScriptManager::GetInstance()->SetErrorString(str);
	}

	char string[4096];
	va_list args;
	va_start(args, aString);
	vsprintf_s(string, aString, args);
	std::cout << string << std::endl;
	va_end(args);

	SetConsoleTextAttribute(hConsole, consoleInfo.wAttributes);
}
void OpenFileWatch(const std::string& aFile)
{
	ScriptManager::GetInstance()->OpenFile(aFile);
}
void CloseFileWatch(const std::string& aFile)
{
	ScriptManager::GetInstance()->CloseFile(aFile);
}

ScriptManager* ScriptManager::ourInstance = nullptr;
void ScriptManager::CreateInstance()
{
	if (ourInstance == nullptr)
	{
		ourInstance = new ScriptManager();
	}
}
ScriptManager* ScriptManager::GetInstance()
{
	return ourInstance;
}
void ScriptManager::DestroyInstance()
{
	if (ourInstance != nullptr)
	{
		delete ourInstance;
		ourInstance = nullptr;
	}
}

SQInteger ScriptManager::SquirrelFunction(HSQUIRRELVM aVM)
{
	SQStackInfos si;
	sq_stackinfos(aVM, 0, &si);

	ScriptFunction* sfPtr = ourInstance->myFunctions.Get(si.funcname);
	/*if (sfPtr == nullptr)
	{
		sfPtr = ourInstance->mySpecialFunctions.Get(si.funcname);
	}*/
	if (sfPtr == nullptr && ourInstance->myRunningScriptStack.size() > 0)
	{
		std::vector<ScriptFunction>* functions = ourInstance->myScriptFunctions.Get(ourInstance->myRunningScriptStack.back());
		if (functions != nullptr)
		{
			const unsigned long long funcHash = CU::Hash(std::string(si.funcname));
			for (size_t funcIndex = 0; sfPtr == nullptr && funcIndex < functions->size(); funcIndex++)
			{
				if (CU::Hash((*functions)[funcIndex].myName) == funcHash)
				{
					sfPtr = &(*functions)[funcIndex];
				}
			}
		}
	}
	assert(sfPtr != nullptr && "Squirrel function not found");
	if (sfPtr == nullptr)
	{
		CU::Log("Squirrel function not found: " + std::string(si.funcname), CU::LogType::Error);
		return 0;
	}

	std::vector<std::any> argumentList;
	std::vector<std::any> returnList;
	SQInteger lastArgumentIndex = sq_gettop(aVM);
	SQInteger integerArg;
	SQFloat floatArg;
	SQBool boolArg;
	const SQChar* stringArg;
	std::any argument;
	for (int argumentIndex = 2; argumentIndex <= lastArgumentIndex; argumentIndex++)
	{
		switch (sq_gettype(aVM, argumentIndex))
		{
		case OT_BOOL:
			sq_getbool(aVM, argumentIndex, &boolArg);
			argument = static_cast<bool>(boolArg);
			break;
		case OT_INTEGER:
			sq_getinteger(aVM, argumentIndex, &integerArg);
			argument = static_cast<int>(integerArg);
			break;
		case OT_FLOAT:
			sq_getfloat(aVM, argumentIndex, &floatArg);
			argument = static_cast<float>(floatArg);
			break;
		case OT_STRING:
			sq_getstring(aVM, argumentIndex, &stringArg);
			argument = std::string(stringArg);
			break;
		default:
			return sq_throwerror(aVM, "invalid parameter");
		}

		argumentList.push_back(argument);
	}

	sfPtr->myFunction(argumentList, returnList);

	if (returnList.size() > 0)
	{
		std::string stringArg;
		for (size_t retIndex = 0; retIndex < returnList.size(); retIndex++)
		{
			if (returnList[retIndex].type() == typeid(float))
			{
				sq_pushfloat(aVM, SafeCast<float>(returnList[retIndex]));
			}
			else if (returnList[retIndex].type() == typeid(int))
			{
				sq_pushinteger(aVM, SafeCast<int>(returnList[retIndex]));
			}
			else if (returnList[retIndex].type() == typeid(std::string))
			{
				stringArg = SafeCast<std::string>(returnList[retIndex]);
				sq_pushstring(aVM, stringArg.c_str(), stringArg.length());
			}
			else if (returnList[retIndex].type() == typeid(bool))
			{
				sq_pushbool(aVM, SafeCast<bool>(returnList[retIndex]));
			}
			else if (returnList[retIndex].type() == typeid(char*))
			{
				sq_pushstring(aVM, SafeCast<char*>(returnList[retIndex]), -1);
			}
		}
		//sq_pushinteger(aVM, static_cast<int>(returnList.size()));
		return static_cast<int>(returnList.size());
	}
	return 0;
}

ScriptManager::ScriptManager() : myScripts(32), myFunctions(64), mySpecialFunctions(24), myScriptFunctions(32)
{
	RegisterFunction(std::bind(&ScriptManager::RegisterEventListenerSquirrel, this, std::placeholders::_1, std::placeholders::_2), "RegisterEventListener");
	RegisterFunction(std::bind(&ScriptManager::UnregisterEventListenerSquirrel, this, std::placeholders::_1, std::placeholders::_2), "UnregisterEventListener");
}
ScriptManager::~ScriptManager()
{
	/*for (CU::HashMap<std::string, ScriptFile>::Iterator scriptIterator = myScripts.Begin(); !scriptIterator.IsEnd(); scriptIterator++)
	{
		sq_close(scriptIterator.GetValue().mySquirrelVM);
	}*/
}

void ScriptManager::AddFunctionToScript(const std::string& aFunctionName, HSQUIRRELVM& aScriptVM)
{
	sq_pushroottable(aScriptVM);
	sq_pushstring(aScriptVM, aFunctionName.c_str(), aFunctionName.length());
	sq_newclosure(aScriptVM, SquirrelFunction, 0);
	sq_setnativeclosurename(aScriptVM, -1, aFunctionName.c_str());
	sq_newslot(aScriptVM, -3, false);
	sq_pop(aScriptVM, 1);
}

long long ScriptManager::RegisterEventListenerSquirrel(const std::vector<std::any>& aArumentList, std::vector<std::any>& aOutReturnList)
{
	aOutReturnList;
	RegisterEventListener(SafeCast<int>(aArumentList[0]), SafeCast<std::string>(aArumentList[1]), SafeCast<std::string>(aArumentList[2]));
	return 0;
}
long long ScriptManager::UnregisterEventListenerSquirrel(const std::vector<std::any>& aArumentList, std::vector<std::any>& aOutReturnList)
{
	aOutReturnList;
	UnregisterEventListener(SafeCast<int>(aArumentList[0]), SafeCast<std::string>(aArumentList[1]), SafeCast<std::string>(aArumentList[2]));
	return 0;
}

void ScriptManager::OpenFile(const std::string& aFile, const std::string& aClass)
{
	ScriptFile* script = myScripts.Get(aFile);

	assert(script != nullptr && "Script not registered");
	if (script == nullptr)
	{
		CU::Log("Script not registered, couldn't open file: " + aFile, CU::LogType::Error);
		return;
	}

	#ifndef _RETAIL
	std::ifstream file(aFile);
	if (std::count(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), '\n') > 100)
	{
		CU::Log("Too many lines in script \"" + aFile + '\"', CU::LogType::Warning);
	}
	CU::Time::StartStopWatch();
	#endif

	if (script->myFileOpen)
	{
		script->myFileOpen = false;
		sq_close(script->mySquirrelVM);
	}
	script->mySquirrelVM = sq_open(1024);

	sqstd_seterrorhandlers(script->mySquirrelVM);
	sq_pushroottable(script->mySquirrelVM);
	sq_setprintfunc(script->mySquirrelVM, Print, ErrorPrint);

	sqstd_register_bloblib(script->mySquirrelVM);
	sqstd_register_iolib(script->mySquirrelVM);
	sqstd_register_systemlib(script->mySquirrelVM);
	sqstd_register_mathlib(script->mySquirrelVM);
	sqstd_register_stringlib(script->mySquirrelVM);

	for (CU::HashMap<std::string, ScriptFunction>::Iterator functionIterator = myFunctions.Begin(); !functionIterator.IsEnd(); functionIterator++)
	{
		AddFunctionToScript(functionIterator.GetKey(), script->mySquirrelVM);
	}
	/*for (int functionIndex = 0; functionIndex < script->myFunctions.size(); functionIndex++)
	{
		AddFunctionToScript(script->myFunctions[functionIndex], script->mySquirrelVM);
	}*/
	const std::vector<ScriptFunction>* functions = ourInstance->myScriptFunctions.Get(aFile);
	if (functions != nullptr)
	{
		for (size_t functionIndex = 0; functionIndex < functions->size(); functionIndex++)
		{
			AddFunctionToScript((*functions)[functionIndex].myName, script->mySquirrelVM);
		}
	}

	myRunningScriptStack.push_back(aFile);
	const SQRESULT result = sqstd_dofile(script->mySquirrelVM, aFile.c_str(), 0, 1);
	if (SQ_FAILED(result))
	{
		PrintError();
		CU::Log("Couldn't open script \"" + aFile + '\"', CU::LogType::Error);
		sq_close(script->mySquirrelVM);
		script->myFileOpen = false;
	}
	else
	{
		if (aClass.length() > 0)
		{
			CallFunction(aClass, aFile);
		}

		//CU::Log("Opened script \"" + aFile + '\"');
		script->myFileOpen = true;
	}
	myRunningScriptStack.pop_back();

	#ifndef _RETAIL
	const float time = CU::Time::EndStopWatch();
	if (time > 1.0f)
	{
		CU::Log("Script \"" + aFile + "\" took long time to execute while opening (" + std::to_string(time) + "ms)", CU::LogType::Warning);

		/*HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
		GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

		std::cout << "Script \"" << aFile << "\" took long time to execute while opening (" << time << "ms)" << std::endl;

		SetConsoleTextAttribute(hConsole, consoleInfo.wAttributes);*/
	}
	#endif
}

void ScriptManager::CloseFile(const std::string& aFile)
{
	ScriptFile* script = myScripts.Get(aFile);

	assert(script != nullptr && "Script not registered");
	if (script == nullptr)
	{
		CU::Log("Script not registered, couldn't close file: " + aFile, CU::LogType::Error);
		return;
	}
	script->myFileOpen = false;
	sq_close(script->mySquirrelVM);
	script->mySquirrelVM = nullptr;
}

void ScriptManager::RegisterScript(const std::string& aFile)
{
	if (!myScripts.Create(aFile))
	{
		assert(false && "Script failed to register");
		CU::Log("Script failed to register: " + aFile, CU::LogType::Error);
		return;
	}

	#ifndef _RETAIL
	CU::FileWatcher::GetInstance()->AddCallback(aFile, std::bind(OpenFileWatch, aFile), std::bind(CloseFileWatch, aFile));
	#endif
}

void ScriptManager::RegisterFunction(const std::function<long long(const std::vector<std::any>&, std::vector<std::any>&)>& aFunction, const std::string& aFunctionName)
{
	if (aFunctionName.length() < 1 || !myFunctions.Create(aFunctionName))
	{
		return;
	}
	ScriptFunction* sfPtr = myFunctions.Get(aFunctionName);
	if (sfPtr == nullptr)
	{
		CU::Log("Failed to find function called \"" + aFunctionName + "\" while trying to register a function", CU::LogType::Error);
		return;
	}
	sfPtr->myFunction = aFunction;
	sfPtr->myName = aFunctionName;

	for (CU::HashMap<std::string, ScriptFile>::Iterator scriptIterator = myScripts.Begin(); !scriptIterator.IsEnd(); scriptIterator++)
	{
		if (scriptIterator.GetValue().myFileOpen)
		{
			AddFunctionToScript(aFunctionName, scriptIterator.GetValue().mySquirrelVM);
		}
	}
}

void ScriptManager::RegisterFunction(const std::function<long long(const std::vector<std::any>&, std::vector<std::any>&)>& aFunction, const std::string& aFunctionName, const std::string& aScript)
{
	if (aFunctionName.length() < 1)
	{
		return;
	}
	ScriptFile* script = myScripts.Get(aScript);
	if (script == nullptr)
	{
		CU::Log("Failed to find script called \"" + aScript + "\" while trying to register a function", CU::LogType::Error);
		return;
	}
	/*mySpecialFunctions.Create(aFunctionName);
	ScriptFunction* sfPtr = mySpecialFunctions.Get(aFunctionName);
	if (sfPtr == nullptr)
	{
		return;
	}*/
	std::vector<ScriptFunction>* functionList = myScriptFunctions.Get(aScript);
	if (functionList == nullptr)
	{
		if (!myScriptFunctions.Create(aScript))
		{
			CU::Log("Failed to create script function entry while trying to register a function", CU::LogType::Error);
			return;
		}
		functionList = myScriptFunctions.Get(aScript);
	}
	functionList->push_back(ScriptFunction(aFunction, aFunctionName));

	//script->myFunctions.push_back(aFunctionName);
	if (script->myFileOpen)
	{
		AddFunctionToScript(aFunctionName, script->mySquirrelVM);
	}
}

void ScriptManager::RegisterEventListener(const int aEvent, const std::string& aFunctionName, const std::string& aScript)
{
	const EventFunction event(aFunctionName, aScript, aEvent);
	if (myEvents.size() <= aEvent)
	{
		myEvents.resize(aEvent + 1);
	}
	myEvents[aEvent].push_back(event);
}

void ScriptManager::RegisterEventListener(const int aEvent, const std::string& aFunctionName, NodeInstance* aNode)
{
	const EventFunction event(aFunctionName, aNode, aEvent);
	if (myEvents.size() <= aEvent)
	{
		myEvents.resize(aEvent + 1);
	}
	myEvents[aEvent].push_back(event);
}

void ScriptManager::UnregisterEventListener(const int aEvent, const std::string& aFunctionName, const std::string& aScript)
{
	if (aEvent < 0 || aEvent >= myEvents.size())
	{
		return;
	}

	for (int functionIndex = 0; functionIndex < myEvents[aEvent].size(); functionIndex++)
	{
		EventFunction& event = myEvents[aEvent][functionIndex];
		if (event.myEvent == aEvent && event.myFunction == aFunctionName && event.myScript == aScript)
		{
			myEvents[aEvent][functionIndex] = myEvents[aEvent][myEvents[aEvent].size() - 1];
			myEvents[aEvent].pop_back();
			break;
		}
	}
}
void ScriptManager::UnregisterEventListener(const int aEvent, const std::string& aFunctionName, NodeInstance* aNode)
{
	if (aEvent < 0 || aEvent >= myEvents.size())
	{
		return;
	}

	for (int functionIndex = 0; functionIndex < myEvents[aEvent].size(); functionIndex++)
	{
		EventFunction& event = myEvents[aEvent][functionIndex];
		if (event.myEvent == aEvent && event.myFunction == aFunctionName && event.myNode == aNode)
		{
			myEvents[aEvent][functionIndex] = myEvents[aEvent][myEvents[aEvent].size() - 1];
			myEvents[aEvent].pop_back();
			break;
		}
	}
}

void ScriptManager::CallFunction(const std::string& aFunctionName)
{
	CallFunction(aFunctionName, std::vector<std::any>());
}
std::vector<std::any> ScriptManager::CallFunction(const std::string& aFunctionName, const std::string& aScript)
{
	return CallFunction(aFunctionName, std::vector<std::any>(), aScript);
}

void ScriptManager::CallFunction(const std::string& aFunctionName, const std::vector<std::any>& aArguments)
{
	if (aFunctionName.length() < 1)
	{
		return;
	}

	#ifndef _RETAIL
	CU::Time::StartStopWatch();
	#endif

	for (CU::HashMap<std::string, ScriptFile>::Iterator scriptIterator = myScripts.Begin(); !scriptIterator.IsEnd(); scriptIterator++)
	{
		ScriptFile& script = scriptIterator.GetValue();
		if (!script.myFileOpen)
		{
			continue;
		}

		const SQInteger top = sq_gettop(script.mySquirrelVM);

		sq_pushroottable(script.mySquirrelVM);
		sq_pushstring(script.mySquirrelVM, aFunctionName.c_str(), aFunctionName.length());

		std::string stringArg;
		if (SQ_SUCCEEDED(sq_get(script.mySquirrelVM, -2)))
		{
			sq_pushroottable(script.mySquirrelVM);
			if (aArguments.size() > 0)
			{
				for (size_t argIndex = 0; argIndex < aArguments.size(); argIndex++)
				{
					if (aArguments[argIndex].type() == typeid(float))
					{
						sq_pushfloat(script.mySquirrelVM, SafeCast<float>(aArguments[argIndex]));
					}
					else if (aArguments[argIndex].type() == typeid(int))
					{
						sq_pushinteger(script.mySquirrelVM, SafeCast<int>(aArguments[argIndex]));
					}
					else if (aArguments[argIndex].type() == typeid(std::string))
					{
						stringArg = SafeCast<std::string>(aArguments[argIndex]);
						sq_pushstring(script.mySquirrelVM, stringArg.c_str(), stringArg.length());
					}
					else if (aArguments[argIndex].type() == typeid(bool))
					{
						sq_pushbool(script.mySquirrelVM, SafeCast<bool>(aArguments[argIndex]));
					}
					else if (aArguments[argIndex].type() == typeid(char*))
					{
						sq_pushstring(script.mySquirrelVM, SafeCast<char*>(aArguments[argIndex]), -1);
					}
					else
					{
						sq_pushnull(script.mySquirrelVM);

						CU::Log("Function called \"" + aFunctionName + "\" will be called with invalid argument from C++", CU::LogType::Error);
					}
				}
			}
		}
		else
		{
			sq_settop(script.mySquirrelVM, top);
		}

		const SQRESULT result = sq_call(script.mySquirrelVM, static_cast<int>(aArguments.size()) + 1, SQTrue, SQTrue);
		if (SQ_FAILED(result))
		{
			PrintError();
		}
		sq_settop(script.mySquirrelVM, top);
	}

	#ifndef _RETAIL
	const float time = CU::Time::EndStopWatch();
	if (time > 1.0f)
	{
		CU::Log("Functions called \"" + aFunctionName + "\" took long time to execute (" + std::to_string(time) + "ms)", CU::LogType::Warning);
	}
	#endif
}

std::vector<std::any> ScriptManager::CallFunction(const std::string& aFunctionName, const std::vector<std::any>& aArguments, const std::string& aScript, const std::string& aClass)
{
	if (aFunctionName.length() < 1)
	{
		return std::vector<std::any>();
	}

	ScriptFile* script = myScripts.Get(aScript);
	if (script == nullptr)
	{
		CU::Log("Failed to find script called \"" + aScript + "\" while trying to call a function", CU::LogType::Error);
		return std::vector<std::any>();
	}

	#ifndef _RETAIL
	CU::Time::StartStopWatch();
	#endif

	const SQInteger top = sq_gettop(script->mySquirrelVM);

	sq_pushroottable(script->mySquirrelVM);
	if (aClass.length() > 0)
	{
		sq_pushstring(script->mySquirrelVM, aClass.c_str(), aClass.length());
		sq_get(script->mySquirrelVM, 2);
		sq_createinstance(script->mySquirrelVM, 1);
	}
	sq_pushstring(script->mySquirrelVM, aFunctionName.c_str(), aFunctionName.length());

	std::string stringArg;
	if (SQ_SUCCEEDED(sq_get(script->mySquirrelVM, -2)))
	{
		sq_pushroottable(script->mySquirrelVM);
		if (aArguments.size() > 0)
		{
			for (size_t argIndex = 0; argIndex < aArguments.size(); argIndex++)
			{
				if (aArguments[argIndex].type() == typeid(float))
				{
					sq_pushfloat(script->mySquirrelVM, SafeCast<float>(aArguments[argIndex]));
				}
				else if (aArguments[argIndex].type() == typeid(int))
				{
					sq_pushinteger(script->mySquirrelVM, SafeCast<int>(aArguments[argIndex]));
				}
				else if (aArguments[argIndex].type() == typeid(std::string))
				{
					stringArg = SafeCast<std::string>(aArguments[argIndex]);
					sq_pushstring(script->mySquirrelVM, stringArg.c_str(), stringArg.length());
				}
				else if (aArguments[argIndex].type() == typeid(bool))
				{
					sq_pushbool(script->mySquirrelVM, SafeCast<bool>(aArguments[argIndex]));
				}
				else if (aArguments[argIndex].type() == typeid(char*))
				{
					sq_pushstring(script->mySquirrelVM, SafeCast<char*>(aArguments[argIndex]), -1);
				}
				else
				{
					sq_pushnull(script->mySquirrelVM);

					#ifndef _RETAIL
					CU::Log("Function called \"" + aFunctionName + "\" in \"" + aScript + "\" will be called with invalid argument from C++", CU::LogType::Error);
					#endif
				}
			}
		}
	}
	else
	{
		sq_settop(script->mySquirrelVM, top);
	}

	const SQInteger preCallTop = sq_gettop(script->mySquirrelVM);
	myRunningScriptStack.push_back(aScript);
	const SQRESULT result = sq_call(script->mySquirrelVM, static_cast<int>(aArguments.size()) + 1, SQTrue, SQTrue);
	if (SQ_FAILED(result))
	{
		PrintError();
	}
	myRunningScriptStack.pop_back();
	const SQInteger postCallTop = sq_gettop(script->mySquirrelVM);

	//const char* returnedString = nullptr;
	//result = sq_getstring(script->mySquirrelVM, postCallTop, &returnedString);
	std::vector<std::any> returnValues;
	//if (SQ_SUCCEEDED(result))
	{
		SQInteger integerArg;
		SQFloat floatArg;
		SQBool boolArg;
		const SQChar* stringArg;
		for (SQInteger stackIndex = postCallTop; stackIndex > top; stackIndex--)
		{
			switch (sq_gettype(script->mySquirrelVM, stackIndex))
			{
			case OT_BOOL:
				sq_getbool(script->mySquirrelVM, stackIndex, &boolArg);
				returnValues.push_back(static_cast<bool>(boolArg));
				break;
			case OT_INTEGER:
				sq_getinteger(script->mySquirrelVM, stackIndex, &integerArg);
				returnValues.push_back(static_cast<int>(integerArg));
				break;
			case OT_FLOAT:
				sq_getfloat(script->mySquirrelVM, stackIndex, &floatArg);
				returnValues.push_back(static_cast<float>(floatArg));
				break;
			case OT_STRING:
				sq_getstring(script->mySquirrelVM, stackIndex, &stringArg);
				returnValues.push_back(std::string(stringArg));
				break;
			default:
				break;
			}
		}
	}
	sq_settop(script->mySquirrelVM, top);

	#ifndef _RETAIL
	const float time = CU::Time::EndStopWatch();
	if (time > 1.0f)
	{
		CU::Log("Function called \"" + aFunctionName + "\" in \"" + aScript + "\" took long time to execute (" + std::to_string(time) + "ms)", CU::LogType::Warning);
	}
	#endif
	return returnValues;
}

void ScriptManager::CallEvent(const int aEvent)
{
	if (aEvent < 0 || aEvent >= myEvents.size())
	{
		return;
	}

	for (int functionIndex = 0; functionIndex < myEvents[aEvent].size(); functionIndex++)
	{
		NodeInstance* node = myEvents[aEvent][functionIndex].myNode;
		if (node == nullptr)
		{
			CallFunction(myEvents[aEvent][functionIndex].myFunction, myEvents[aEvent][functionIndex].myScript);
		}
		else
		{
			node->Event(myEvents[aEvent][functionIndex].myFunction);
		}
	}
}

void ScriptManager::SetErrorString(const std::string& aString)
{
	myErrorString = aString;
}

void ScriptManager::PrintError()
{
	#ifndef _RETAIL
	if (myErrorString.length() < 1)
	{
		return;
	}

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);

	size_t funcStart = myErrorString.find_first_of('\'') + 1;
	size_t funcEnd = myErrorString.find_first_of('\'', funcStart);
	std::string errorString = myErrorString.substr(funcStart, funcEnd - funcStart);
	if (myFunctions.Get(errorString) == nullptr)
	{
		std::cout << "Unknown identifier \"" << errorString << "\".";
		unsigned int minDist = 5;
		std::string closestString;
		for (CU::HashMap<std::string, ScriptFunction>::Iterator functionIterator = myFunctions.Begin(); !functionIterator.IsEnd(); functionIterator++)
		{
			unsigned int levDist = CU::LevenshteinDistance(errorString, functionIterator.GetKey());
			if (levDist < minDist)
			{
				minDist = levDist;
				closestString = functionIterator.GetKey();
			}
			//std::cout << levDist << functionIterator.GetKey() << std::endl;
		}
		if (closestString.length() > 0)
		{
			std::cout << " Did you mean \"" << closestString << "\"?";
		}
		std::cout << std::endl;
	}

	SetConsoleTextAttribute(hConsole, consoleInfo.wAttributes);
	#endif
}