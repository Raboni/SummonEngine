#pragma once
#include "NodePin.h"
#include "GrowingArray.hpp"
#include <vector>
#include <string>
#include <any>

struct ScriptFile;
class NodeSystem;
class NodeInstance;

enum class VisualScriptType
{
	Native,
	Ralmark
};

class NodeScript
{
public:
	NodeScript();
	~NodeScript();

private:
	friend class NodeSystem;
	friend class NodeInstance;

	NodeScript(const std::string& aScriptPath, const VisualScriptType aVisualScriptType = VisualScriptType::Native);

	void Run(NodeInstance* aInstance);
	void Run(NodeInstance* aInstance, const std::vector<std::any>& aArguments);
	void Event(NodeInstance* aInstance, const std::string& aFunction);

	bool ParseFile(const std::string& aScriptPath, const VisualScriptType aVisualScriptType = VisualScriptType::Native);

	long long CreateOutput(const std::vector<std::any>& aArumentList, std::vector<std::any>& aOutReturnList);
	long long RegisterEventListenerSquirrel(const std::vector<std::any>& aArumentList, std::vector<std::any>& aOutReturnList);
	long long UnregisterEventListenerSquirrel(const std::vector<std::any>& aArumentList, std::vector<std::any>& aOutReturnList);

private:
	CU::GrowingArray<NodeInstance*> myStackedRunningInstances;
	CU::GrowingArray<NodePin> myPinsIn;
	CU::GrowingArray<NodePin> myPinsOut;
	std::string myName;
	std::string myScriptPath;
	NodeSystem* mySystem;
	bool myHasRun;
};