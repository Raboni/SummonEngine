#include "NodeSystem.h"
#include "ScriptManager.h"
#include <filesystem>
#include <tinyxml2.h>
#ifndef _RETAIL
#include <DebugTools.h>
#endif

NodeSystem::NodeSystem(const std::string& aNodeFolderPath, const int aNodeTypeMaxCount, const int aNodeInstanceMaxCount, const VisualScriptType aVisualScriptType)
	: myVisualScriptType(aVisualScriptType), myNodeScripts(aNodeTypeMaxCount), myNodeInstances(aNodeInstanceMaxCount)
{
	myNodeFolderPath = aNodeFolderPath;
	const std::filesystem::path nodeFolderPath(myNodeFolderPath);
	for (std::filesystem::path nodePath : std::filesystem::directory_iterator(nodeFolderPath))
	{
		AddNodeType(nodePath.string());
	}
}
NodeSystem::~NodeSystem()
{
}

void NodeSystem::AddNodeType(const std::string& aFilePath)
{
	const std::string name = std::filesystem::path(aFilePath).stem().string();
	if (myNodeScripts.Create(name))
	{
		ScriptManager::GetInstance()->RegisterScript(aFilePath);
	}
	NodeScript* script = myNodeScripts.Get(name);
	if (script == nullptr || !script->ParseFile(aFilePath, myVisualScriptType))
	{
		#ifndef _RETAIL
		CU::Log("NodeScript \"" + name + "\" could not be created", CU::LogType::Error);
		#endif
		return;
	}
	script->mySystem = this;
	ScriptManager::GetInstance()->OpenFile(aFilePath);
}

NodeInstance* NodeSystem::CreateInstance(const std::string& aScriptName)
{
	NodeScript* node = myNodeScripts.Get(aScriptName);
	assert(myNodeInstances.Size() != myNodeInstances.Capacity() && "NodeInstanceMaxCount reached");
	if (node == nullptr || myNodeInstances.Size() == myNodeInstances.Capacity())
	{
		#ifndef _RETAIL
		CU::Log("Failed to create node instance for: " + aScriptName, CU::LogType::Error);
		#endif
		return nullptr;
	}
	myNodeInstances.Add(NodeInstance(node));
	return &myNodeInstances.GetLast();
}

NodeInstance* NodeSystem::LoadVisualScript(const std::string& aFilePath)
{
	NodeInstance* startNode = nullptr;
	tinyxml2::XMLDocument doc;
	const tinyxml2::XMLError loadError = doc.LoadFile(aFilePath.c_str());
	if (loadError != tinyxml2::XMLError::XML_SUCCESS)
	{
		#ifndef _RETAIL
		CU::Log("Failed to load visual script with error: " + std::to_string(loadError), CU::LogType::Error);
		#endif
		return nullptr;
	}

	std::string nodeName;
	tinyxml2::XMLElement* root = doc.RootElement();
	if (root == nullptr)
	{
		return nullptr;
	}
	tinyxml2::XMLElement* nameElement = nullptr;
	tinyxml2::XMLElement* IDElement = nullptr;
	NodeInstance* instance = nullptr;
	tinyxml2::XMLElement* connection = nullptr;
	tinyxml2::XMLElement* pinElement = nullptr;
	tinyxml2::XMLElement* node = root->FirstChildElement("Node");
	CU::GrowingArray<NodeInstance*> instanceList(16);
	CU::GrowingArray<long long> instanceIDList(16);

	while (node != nullptr)
	{
		nameElement = node->FirstChildElement("Name");
		nodeName = nameElement->GetText();
		if (myVisualScriptType == VisualScriptType::Ralmark)
		{
			nameElement = node->FirstChildElement("FilePath");
			const std::string filename = std::filesystem::path(nameElement->GetText()).stem().string();
			instance = CreateInstance(filename);
		}
		else
		{
			instance = CreateInstance(nodeName);
		}
		if (instance == nullptr)
		{
			#ifndef _RETAIL
			CU::Log("Failed to create node \"" + nodeName + '\"', CU::LogType::Error);
			#endif
			node = node->NextSiblingElement("Node");
			continue;
		}
		else if (startNode == nullptr && ((myVisualScriptType == VisualScriptType::Ralmark && nodeName == "Start") || nodeName == "StartNode"))
		{
			startNode = instance;
		}
		IDElement = node->FirstChildElement("UID");
		instanceList.Add(instance);
		instanceIDList.Add(IDElement->Int64Text());

		connection = node->FirstChildElement("Connections")->FirstChildElement("Connection");
		while (connection != nullptr)
		{
			pinElement = connection->FirstChildElement("Index");
			const int pinIndex = pinElement->IntText();
			pinElement = connection->FirstChildElement("VarType");
			const std::string pinType = pinElement->GetText();
			pinElement = connection->FirstChildElement("Type");
			const bool input = (std::string(pinElement->GetText()) == "Input");
			pinElement = connection->FirstChildElement("PinConnection");
			if (pinElement != nullptr)
			{
				pinElement = pinElement->FirstChildElement("ConnectedTo");
				const long long id = pinElement->Int64Text();
				pinElement = pinElement->NextSiblingElement("PinIndex");
				const int otherPinIndex = pinElement->IntText();
				for (int instanceIndex = instanceIDList.Size() - 1; instanceIndex >= 0; instanceIndex--)
				{
					if (id == instanceIDList[instanceIndex])
					{
						//since Ralmark has no run for start node
						if (myVisualScriptType == VisualScriptType::Ralmark && startNode != nullptr)
						{
							const bool isStart = (startNode == instance);
							bool otherIsStart = false;
							for (int startSearchIndex = 0; startSearchIndex < instanceIDList.Size(); startSearchIndex++)
							{
								if (instanceList[startSearchIndex] == startNode)
								{
									otherIsStart = (id == instanceIDList[startSearchIndex]);
									break;
								}
							}

							if (input)
							{
								if (isStart)
								{
									continue;
								}
								instanceList[instanceIndex]->ConnectPinOut(instance, pinIndex, otherPinIndex, !otherIsStart);
							}
							else
							{
								if (otherIsStart)
								{
									continue;
								}
								instanceList[instanceIndex]->ConnectPinIn(instance, pinIndex, otherPinIndex, !isStart);
							}
							continue;
						}

						if (input)
						{
							instanceList[instanceIndex]->ConnectPinOut(instance, pinIndex, otherPinIndex, true);
						}
						else
						{
							instanceList[instanceIndex]->ConnectPinIn(instance, pinIndex, otherPinIndex, true);
						}
					}
				}
			}
			else if (input)
			{
				pinElement = connection->FirstChildElement("PinData");
				if (pinElement != nullptr)
				{
					switch (pinType[0])
					{
					case 'i':
						if (pinType == "int")
						{
							instance->SetPinInData(pinElement->IntText(), pinIndex);
						}
						break;
					case 'f':
						if (pinType == "float")
						{
							instance->SetPinInData(pinElement->FloatText(), pinIndex);
						}
						break;
					case 's':
						if (pinType == "string")
						{
							instance->SetPinInData(std::string(pinElement->GetText()), pinIndex);
						}
						break;
					case 'b':
						if (pinType == "bool")
						{
							instance->SetPinInData(pinElement->BoolText(), pinIndex);
						}
						break;
					case 'c':
						if (pinType == "char")
						{
							instance->SetPinInData(static_cast<char>(pinElement->IntText()), pinIndex);
						}
						break;
					default:
						break;
					}
				}
			}

			connection = connection->NextSiblingElement("Connection");
		}

		node = node->NextSiblingElement("Node");
	}

	return startNode;
}

NodeInstance* NodeSystem::GetCurrentRunningInstance()
{
	return myCurrentRunningInstance;
}