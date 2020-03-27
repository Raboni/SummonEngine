#pragma once
#include "NodeScript.h"
#include "NodeConnection.h"
#include <OpaqueDictionary.hpp>
#include <vector>
#include <string>
#include <any>

class NodeInstance
{
public:
	NodeInstance();
	NodeInstance(NodeScript* aNodeScript);
	~NodeInstance();

	void Run();
	void Run(const std::vector<std::any>& aArguments);
	void Event(const std::string& aFunction);

	void SetPinInData(const std::any& aData, const int aPinIndex);
	void ConnectPinIn(NodeInstance* aOtherInstance, const int aOtherPinIndex, const int aMyPinIndex, const bool aSeparateInputOutputPinIndex = false);
	void ConnectPinOut(NodeInstance* aOtherInstance, const int aOtherPinIndex, const int aMyPinIndex, const bool aSeparateInputOutputPinIndex = false);

	const int GetInputPinCount();
	const int GetOutputPinCount();

	std::vector<std::any> GetInput();
	template<typename T>
	const T& GetOutput(const int aPinIndex);
	template<typename T>
	const T& GetOutput(const std::string& aPinName);

private:
	friend class NodeScript;

	CU::OpaqueDictionary<std::string> myOutput;
	CU::GrowingArray<NodeConnection> myConnectionsIn;
	NodeScript* myNodeScript;
};

template<typename T>
inline const T& NodeInstance::GetOutput(const int aPinIndex)
{
	std::string pinName = myNodeScript->myPinsOut[aPinIndex].myName;
	return GetOutput<T>(pinName);
}

template<typename T>
inline const T& NodeInstance::GetOutput(const std::string& aPinName)
{
	if (myNodeScript->myPinsIn.Size() > 0)
	{
		//if has run return output, otherwise get input and run script
		if (!myNodeScript->myHasRun)
		{
			Run(GetInput());
		}
		return *myOutput.GetValue<T>(aPinName);
	}
	else
	{
		//run this script and return output
		Run(std::vector<std::any>());
		return *myOutput.GetValue<T>(aPinName);
	}
}
