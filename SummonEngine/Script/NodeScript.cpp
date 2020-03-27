#include "NodeScript.h"
#include "NodeInstance.h"
#include "NodeSystem.h"
#include "ScriptManager.h"
#include "Horspool.hpp"
#include <fstream>

NodeScript::NodeScript()
{
	myStackedRunningInstances.Init(4);
	myPinsIn.Init(2);
	myPinsOut.Init(2);
	mySystem = nullptr;
}
NodeScript::NodeScript(const std::string& aScriptPath, const VisualScriptType aVisualScriptType)
{
	myPinsIn.Init(2);
	myPinsOut.Init(2);
	ParseFile(aScriptPath, aVisualScriptType);
	mySystem = nullptr;
}
NodeScript::~NodeScript()
{
}

void NodeScript::Run(NodeInstance* aInstance)
{
	Run(aInstance, aInstance->GetInput());
}

void NodeScript::Run(NodeInstance* aInstance, const std::vector<std::any>& aArguments)
{
	mySystem->myCurrentRunningInstance = aInstance;
	myStackedRunningInstances.Add(aInstance);
	std::vector<std::any> returnedValues;
	if (myHasRun || mySystem->myVisualScriptType == VisualScriptType::Ralmark)
	{
		returnedValues = ScriptManager::GetInstance()->CallFunction("Run", aArguments, myScriptPath, myName);
	}
	else
	{
		returnedValues = ScriptManager::GetInstance()->CallFunction("RunOut", aArguments, myScriptPath, myName);
	}
	if (returnedValues.size() > 0)
	{
		CU::GrowingArray<NodeInstance*>* callList = aInstance->myOutput.GetValue<CU::GrowingArray<NodeInstance*>>(SafeCast<std::string>(returnedValues[0]));
		if (callList != nullptr)
		{
			for (int callIndex = 0; callIndex < callList->Size(); callIndex++)
			{
				(*callList)[callIndex]->myNodeScript->Run((*callList)[callIndex]);
			}
		}
	}
	myStackedRunningInstances.RemoveLast();
	mySystem->myCurrentRunningInstance = nullptr;
}

void NodeScript::Event(NodeInstance* aInstance, const std::string& aFunction)
{
	//std::vector<std::any> arguments = aInstance->GetInput();
	mySystem->myCurrentRunningInstance = aInstance;
	myStackedRunningInstances.Add(aInstance);
	std::vector<std::any> returnedValues = ScriptManager::GetInstance()->CallFunction(aFunction, std::vector<std::any>(), myScriptPath, myName);
	if (returnedValues.size() > 0)
	{
		CU::GrowingArray<NodeInstance*>* callList = aInstance->myOutput.GetValue<CU::GrowingArray<NodeInstance*>>(SafeCast<std::string>(returnedValues[0]));
		if (callList != nullptr)
		{
			for (int callIndex = 0; callIndex < callList->Size(); callIndex++)
			{
				(*callList)[callIndex]->myNodeScript->Run((*callList)[callIndex]);
			}
		}
	}
	myStackedRunningInstances.RemoveLast();
	mySystem->myCurrentRunningInstance = nullptr;
}

bool NodeScript::ParseFile(const std::string& aScriptPath, const VisualScriptType aVisualScriptType)
{
	ScriptManager* manager = ScriptManager::GetInstance();
	if (manager == nullptr)
	{
		return false;
	}

	myScriptPath = aScriptPath;

	std::ifstream stream(aScriptPath);
	if (!stream)
	{
		return false;
	}
	std::string fileString;

	int classEndPos = -1;
	{
		char readString[1024];
		while (!stream.eof())
		{
			stream.read(readString, 1024);
			//assert(!stream.fail() && "stream failed to read script file correctly");
			//assert(!stream.bad() && "stream failed to read script file correctly");
			fileString.append(readString, 1024);
		}
		int classStartPos = CU::Horspool("class", fileString);
		if (classStartPos == -1)
		{
			return false;
		}
		classStartPos += 6;
		int classEndLine = static_cast<int>(fileString.find_first_of('\n', classStartPos));
		int classEndSpace = static_cast<int>(fileString.find_first_of(' ', classStartPos));
		classEndPos = classEndLine;
		if (classEndSpace != -1 && classEndSpace < classEndLine)
		{
			classEndPos = classEndSpace;
		}
		myName = fileString.substr(classStartPos, classEndPos - classStartPos);
	}

	{
		int runStartPos = CU::Horspool("function Run", fileString);
		if (runStartPos == -1)
		{
			return false;
		}
		runStartPos += 9;
		if (aVisualScriptType == VisualScriptType::Ralmark)
		{
			std::string grabberString = fileString.substr(classEndPos, 25);
			int grabberStart = CU::Horspool("BaseNodeGrabber", grabberString);
			myHasRun = (grabberStart == -1);
		}
		else
		{
			myHasRun = (fileString.substr(runStartPos + 3, 3) != "Out");
		}
		if (myHasRun)
		{
			myPinsIn.Add(NodePin("Run", PinDataType::Run));
		}
		int argPos = static_cast<int>(fileString.find_first_of('(', runStartPos)) + 1;
		int argEndPos = static_cast<int>(fileString.find_first_of(')', argPos));
		std::string argString(fileString.substr(argPos, argEndPos - argPos));
		if (argString.length() > 0)
		{
			int nextArgPos = -2;
			do
			{
				argPos = nextArgPos + 2;
				nextArgPos = static_cast<int>(argString.find_first_of(", ", argPos));
				if (nextArgPos == -1)
				{
					nextArgPos = static_cast<int>(argString.length());
				}
				myPinsIn.Add(NodePin(argString.substr(argPos, nextArgPos - argPos), PinDataType::None));
			} while (nextArgPos < static_cast<int>(argString.length()));
			int typeStartPos = CU::Horspool("//define argTypes ", fileString);
			int typeEndPos = static_cast<int>(fileString.find_first_of('\n', typeStartPos));
			if (typeStartPos != -1 && typeEndPos != -1)
			{
				typeStartPos += 18;
				argString = fileString.substr(typeStartPos, typeEndPos - typeStartPos);
				std::string argTypeString;
				typeStartPos = 0;
				for (int pinIndex = static_cast<int>(myHasRun); pinIndex < myPinsIn.Size(); pinIndex++)
				{
					if ((typeEndPos = static_cast<int>(argString.find_first_of(',', typeStartPos))) == -1 &&
						(typeEndPos = static_cast<int>(argString.find_first_of('\n', typeStartPos))) == -1 &&
						(typeEndPos = static_cast<int>(argString.find_first_of(' ', typeStartPos))) == -1)
					{
						typeEndPos = static_cast<int>(argString.length());
						//break;
					}
					argTypeString = argString.substr(typeStartPos, typeEndPos - typeStartPos);
					switch (argTypeString[0])
					{
					case 'i':
						if (argTypeString == "int")
						{
							myPinsIn[pinIndex].myDataType = PinDataType::Int;
						}
						break;
					case 'f':
						if (argTypeString == "float")
						{
							myPinsIn[pinIndex].myDataType = PinDataType::Float;
						}
						break;
					case 's':
						if (argTypeString == "string")
						{
							myPinsIn[pinIndex].myDataType = PinDataType::String;
						}
						break;
					case 'b':
						if (argTypeString == "bool")
						{
							myPinsIn[pinIndex].myDataType = PinDataType::Bool;
						}
						break;
					default:
						break;
					}
					typeStartPos = typeEndPos + 1;
				}
			}
			else if (aVisualScriptType == VisualScriptType::Ralmark)
			{
				for (int pinIndex = static_cast<int>(myHasRun); pinIndex < myPinsIn.Size(); pinIndex++)
				{
					myPinsIn[pinIndex].myDataType = PinDataType::String;
				}
			}
		}
	}
	{
		int returnPos = CU::Horspool("return \"", fileString);
		if (returnPos != -1)
		{
			do
			{
				returnPos += 8;
				int returnEndPos = static_cast<int>(fileString.find_first_of('\"', returnPos));
				myPinsOut.Add(NodePin(fileString.substr(returnPos, returnEndPos - returnPos), PinDataType::Call));
				returnPos = CU::Horspool("return \"", fileString, returnEndPos);
			} while (returnPos != -1);
		}
		int outputPos = CU::Horspool("CreateOutput(\"", fileString);
		if (outputPos != -1)
		{
			std::string outputName;
			std::string argTypeString;
			do
			{
				outputPos += 14;
				int outputEndPos = static_cast<int>(fileString.find_first_of('\"', outputPos));
				outputName = fileString.substr(outputPos, outputEndPos - outputPos);

				PinDataType type = PinDataType::None;
				if (aVisualScriptType == VisualScriptType::Ralmark)
				{
					type = PinDataType::String;
				}
				else
				{
					int typeStartPos = static_cast<int>(fileString.find_first_of(',', outputEndPos + 2)) + 3;
					int typeEndPos = static_cast<int>(fileString.find_first_of(')', typeStartPos)) - 1;
					argTypeString = fileString.substr(typeStartPos, typeEndPos - typeStartPos);
					switch (argTypeString[0])
					{
					case 'i':
						if (argTypeString == "int")
						{
							type = PinDataType::Int;
						}
						break;
					case 'f':
						if (argTypeString == "float")
						{
							type = PinDataType::Float;
						}
						break;
					case 's':
						if (argTypeString == "string")
						{
							type = PinDataType::String;
						}
						break;
					case 'b':
						if (argTypeString == "bool")
						{
							type = PinDataType::Bool;
						}
						break;
					default:
						break;
					}
				}

				myPinsOut.Add(NodePin(outputName, type));
				outputPos = CU::Horspool("CreateOutput(\"", fileString, outputEndPos);
			} while (outputPos != -1);
			manager->RegisterFunction(std::bind(&NodeScript::CreateOutput, this, std::placeholders::_1, std::placeholders::_2), "CreateOutput", myScriptPath);
		}
	}
	manager->RegisterFunction(std::bind(&NodeScript::RegisterEventListenerSquirrel, this, std::placeholders::_1, std::placeholders::_2), "RegisterNodeEventListener", myScriptPath);
	manager->RegisterFunction(std::bind(&NodeScript::UnregisterEventListenerSquirrel, this, std::placeholders::_1, std::placeholders::_2), "UnregisterNodeEventListener", myScriptPath);
	return true;
}

long long NodeScript::CreateOutput(const std::vector<std::any>& aArumentList, std::vector<std::any>& aOutReturnList)
{
	if (mySystem->myVisualScriptType == VisualScriptType::Ralmark)
	{
		myStackedRunningInstances.GetLast()->myOutput.SetValue(SafeCast<std::string>(aArumentList[0]), SafeCast<std::string>(aArumentList[1]));
		return 0;
	}
	//checking strings might be slow, need a better solution to find the data type
	std::string dataType = SafeCast<std::string>(aArumentList[2]);
	switch (dataType[0])
	{
	case 'i':
		if (dataType == "int")
		{
			myStackedRunningInstances.GetLast()->myOutput.SetValue(SafeCast<std::string>(aArumentList[0]), SafeCast<int>(aArumentList[1]));
		}
		break;
	case 'f':
		if (dataType == "float")
		{
			myStackedRunningInstances.GetLast()->myOutput.SetValue(SafeCast<std::string>(aArumentList[0]), SafeCast<float>(aArumentList[1]));
		}
		break;
	case 's':
		if (dataType == "string")
		{
			myStackedRunningInstances.GetLast()->myOutput.SetValue(SafeCast<std::string>(aArumentList[0]), SafeCast<std::string>(aArumentList[1]));
		}
		break;
	case 'b':
		if (dataType == "bool")
		{
			myStackedRunningInstances.GetLast()->myOutput.SetValue(SafeCast<std::string>(aArumentList[0]), SafeCast<bool>(aArumentList[1]));
		}
		break;
	default:
		break;
	}
	return 0;
}

long long NodeScript::RegisterEventListenerSquirrel(const std::vector<std::any>& aArumentList, std::vector<std::any>& aOutReturnList)
{
	ScriptManager::GetInstance()->RegisterEventListener(SafeCast<int>(aArumentList[0]), SafeCast<std::string>(aArumentList[1]), myStackedRunningInstances.GetLast());
	return 0;
}
long long NodeScript::UnregisterEventListenerSquirrel(const std::vector<std::any>& aArumentList, std::vector<std::any>& aOutReturnList)
{
	ScriptManager::GetInstance()->UnregisterEventListener(SafeCast<int>(aArumentList[0]), SafeCast<std::string>(aArumentList[1]), myStackedRunningInstances.GetLast());
	return 0;
}