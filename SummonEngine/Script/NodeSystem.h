#pragma once
#include "NodeScript.h"
#include "NodeInstance.h"
#include <HashMap.hpp>
#include <GrowingArrayVector.hpp>
#include <vector>
#include <string>

class NodeSystem
{
public:
	NodeSystem(const std::string& aNodeFolderPath, const int aNodeTypeMaxCount = 16, const int aNodeInstanceMaxCount = 64, const VisualScriptType aVisualScriptType = VisualScriptType::Native);
	~NodeSystem();

	//Adds a new node type. Use this if the NodeType script is outside NodeFolderPath.
	void AddNodeType(const std::string& aFilePath);
	NodeInstance* CreateInstance(const std::string& aScriptName);
	//Retuns the instance of the start node.
	NodeInstance* LoadVisualScript(const std::string& aFilePath);
	NodeInstance* GetCurrentRunningInstance();

private:
	friend class NodeScript;

	CU::HashMap<std::string, NodeScript> myNodeScripts;
	CU::GrowingArray<NodeInstance> myNodeInstances;
	std::string myNodeFolderPath;
	NodeInstance* myCurrentRunningInstance;
	const VisualScriptType myVisualScriptType;
};