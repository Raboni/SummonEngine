#include "NodeInstance.h"
#include "SafeCast.h"
#ifndef _RETAIL
#include <DebugTools.h>
#include <Windows.h>
#endif

NodeInstance::NodeInstance() : myOutput(1)
{
	myNodeScript = nullptr;
}
NodeInstance::NodeInstance(NodeScript* aNodeScript) : myOutput(64)
{
	myNodeScript = aNodeScript;
	for (int pinIndex = 0; pinIndex < myNodeScript->myPinsOut.Size(); pinIndex++)
	{
		bool success = false;
		switch (myNodeScript->myPinsOut[pinIndex].myDataType)
		{
		case PinDataType::Int:
			success = myOutput.SetValue<int>(myNodeScript->myPinsOut[pinIndex].myName, 0);
			break;
		case PinDataType::Float:
			success = myOutput.SetValue<float>(myNodeScript->myPinsOut[pinIndex].myName, 0.0f);
			break;
		case PinDataType::String:
			success = myOutput.SetValue<std::string>(myNodeScript->myPinsOut[pinIndex].myName, "");
			break;
		case PinDataType::Call:
			success = myOutput.SetValue<CU::GrowingArray<NodeInstance*>>(myNodeScript->myPinsOut[pinIndex].myName, CU::GrowingArray<NodeInstance*>(4));
			break;
		case PinDataType::None:
		default:
			break;
		}
		assert(success && "NodeInstance pin creation failed");
	}
	myConnectionsIn.Init(static_cast<int>(myNodeScript->myPinsIn.Size()));
	myConnectionsIn.Resize(myConnectionsIn.Capacity());
}
NodeInstance::~NodeInstance()
{
}

void NodeInstance::Run()
{
	myNodeScript->Run(this, GetInput());
}

void NodeInstance::Run(const std::vector<std::any>& aArguments)
{
	myNodeScript->Run(this, aArguments);
}

void NodeInstance::Event(const std::string& aFunction)
{
	myNodeScript->Event(this, aFunction);
}

void NodeInstance::SetPinInData(const std::any& aData, const int aPinIndex)
{
	myConnectionsIn[aPinIndex].myData = aData;
}

void NodeInstance::ConnectPinIn(NodeInstance* aOtherInstance, const int aOtherPinIndex, const int aMyPinIndex, const bool aSeparateInputOutputPinIndex)
{
	int otherPinIndex = aOtherPinIndex;
	if (aSeparateInputOutputPinIndex)
	{
		otherPinIndex -= aOtherInstance->GetInputPinCount();
	}
	myConnectionsIn[aMyPinIndex].myOtherNode = aOtherInstance;
	myConnectionsIn[aMyPinIndex].myOtherPinIndex = otherPinIndex;
	NodePin outPin = aOtherInstance->myNodeScript->myPinsOut[otherPinIndex];
	if (outPin.myDataType == PinDataType::Call)
	{
		CU::GrowingArray<NodeInstance*>* callList = aOtherInstance->myOutput.GetValue<CU::GrowingArray<NodeInstance*>>(outPin.myName);
		assert(callList != nullptr && "NodeInstance Couldn't find callList");
		if (callList != nullptr)
		{
			callList->Add(this);
		}
	}
	/*else
	{
		aOtherInstance->myOutput.SetValue(aOtherInstance->myNodeScript->myPinsOut[aOtherPinIndex].myName, this);
	}*/
}

void NodeInstance::ConnectPinOut(NodeInstance* aOtherInstance, const int aOtherPinIndex, const int aMyPinIndex, const bool aSeparateInputOutputPinIndex)
{
	int pinIndex = aMyPinIndex;
	if (aSeparateInputOutputPinIndex)
	{
		pinIndex -= GetInputPinCount();
	}
	NodePin outPin = myNodeScript->myPinsOut[pinIndex];
	if (outPin.myDataType == PinDataType::Call)
	{
		CU::GrowingArray<NodeInstance*>* callList = myOutput.GetValue<CU::GrowingArray<NodeInstance*>>(outPin.myName);
		assert(callList != nullptr && "NodeInstance Couldn't find callList");
		if (callList != nullptr)
		{
			callList->Add(aOtherInstance);
		}
	}
	/*else
	{
		myOutput.SetValue(myNodeScript->myPinsOut[aMyPinIndex].myName, aOtherInstance);
	}*/
	aOtherInstance->myConnectionsIn[aOtherPinIndex].myOtherNode = this;
	aOtherInstance->myConnectionsIn[aOtherPinIndex].myOtherPinIndex = aMyPinIndex;
}

const int NodeInstance::GetInputPinCount()
{
	return myConnectionsIn.Size();
}
const int NodeInstance::GetOutputPinCount()
{
	return static_cast<int>(myOutput.GetEntryCount());
}

std::vector<std::any> NodeInstance::GetInput()
{
	std::vector<std::any> arguments;
	for (int connectionIndex = 0; connectionIndex < myConnectionsIn.Size(); connectionIndex++)
	{
		NodePin nodePinIn = myNodeScript->myPinsIn[connectionIndex];
		/*if (myConnectionsIn[connectionIndex].myData != nullptr)
		{
			switch (nodePinIn.myDataType)
			{
			case PinDataType::Int:
				arguments.push_back(*static_cast<int*>(myConnectionsIn[connectionIndex].myData));
				break;
			case PinDataType::Float:
				arguments.push_back(*static_cast<float*>(myConnectionsIn[connectionIndex].myData));
				break;
			case PinDataType::String:
				arguments.push_back(*static_cast<std::string*>(myConnectionsIn[connectionIndex].myData));
				break;
			case PinDataType::Run:
			case PinDataType::None:
			default:
				break;
			}
		}*/
		if (myConnectionsIn[connectionIndex].myData.has_value())
		{
			switch (nodePinIn.myDataType)
			{
			case PinDataType::Int:
			case PinDataType::Float:
			case PinDataType::String:
				arguments.push_back(myConnectionsIn[connectionIndex].myData);
				break;
			case PinDataType::Run:
			case PinDataType::None:
			default:
				break;
			}
		}
		else if (myConnectionsIn[connectionIndex].myOtherNode != nullptr)
		{
			switch (nodePinIn.myDataType)
			{
			case PinDataType::Int:
				arguments.push_back(myConnectionsIn[connectionIndex].myOtherNode->GetOutput<int>(myConnectionsIn[connectionIndex].myOtherPinIndex));
				break;
			case PinDataType::Float:
				arguments.push_back(myConnectionsIn[connectionIndex].myOtherNode->GetOutput<float>(myConnectionsIn[connectionIndex].myOtherPinIndex));
				break;
			case PinDataType::String:
				arguments.push_back(myConnectionsIn[connectionIndex].myOtherNode->GetOutput<std::string>(myConnectionsIn[connectionIndex].myOtherPinIndex));
				break;
			case PinDataType::Run:
			case PinDataType::None:
			default:
				break;
			}
		}
		#ifndef _RETAIL
		else
		{
			CU::Log("The pin \"" + myNodeScript->myPinsIn[connectionIndex].myName + "\" in the node \"" + myNodeScript->myName + "\" has no valid input", CU::LogType::Error);
		}
		#endif
	}
	return arguments;
}