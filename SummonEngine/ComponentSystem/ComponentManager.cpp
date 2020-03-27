#include "Components/stdafx.h"
#include "ComponentManager.h"
#include "Components/Component.h"
#include <thread>
#include <mutex>

//#define MUTEX_NEW_ID

ComponentManager::ComponentManager()
{
	myActiveObjects.reset();
	myComponentData = nullptr;
	myDataSize = 0;
	myLastDataIndex = 0;
	myNextObjectID = 0;
	myConstantObjectAmount = 0;
	myCreationLocked = false;
}
ComponentManager::~ComponentManager()
{
	if (myComponentData != nullptr)
	{
		#ifndef _RETAIL
		LogMaxUsedComponents();
		#endif
		myDataInfo.RemoveAll();
		delete[] myComponentData;
		myComponentData = nullptr;
	}
}
void ComponentManager::Init(const unsigned int aObjectAmount, const unsigned int aConstantObjectAmount)
{
	assert(myComponentData == nullptr && "Component manager already in use");
	assert(aObjectAmount <= ourMaxObjectAmount && "Maximum object amount reached");
	if (myComponentData != nullptr || aObjectAmount > ourMaxObjectAmount)
	{
		return;
	}

	myConstantObjectAmount = aConstantObjectAmount;
	myNextObjectID = aConstantObjectAmount;
	myActiveObjects.reset();
	myObjectIDs.Init(aObjectAmount - aConstantObjectAmount);
	myDataInfo.Init(32);
	myObjectComponents.Init(aObjectAmount);
	myObjectComponents.Resize(aObjectAmount);
	for (int index = 0; index < myObjectComponents.Size(); index++)
	{
		myObjectComponents[index].Init(4);
	}

	#ifndef _RETAIL
	myMaxComponentCountList.Init(32);
	#endif
}
void ComponentManager::Update(const float aDeltaTime)
{
	Component* compPtr = nullptr;
	int compIndex = -1;
	for (int typeIndex = 0; typeIndex < myDataInfo.Size(); typeIndex++)
	{
		if (myDataInfo[typeIndex].myAmount > 0 && myDataInfo[typeIndex].myShouldUpdate)
		{
			for (compIndex = 0; compIndex < myDataInfo[typeIndex].myAmount; compIndex++)
			{
				const int dataIndex = compIndex * myDataInfo[typeIndex].myTypeSize + myDataInfo[typeIndex].myStartIndex;
				compPtr = reinterpret_cast<Component*>(&myComponentData[dataIndex]);
				if (compPtr->GetObjectID() != -1 && myActiveObjects[compPtr->GetObjectID()] && compPtr->GetActive())
				{
					compPtr->Update(aDeltaTime);
				}
			}
		}
	}
}

Component* ComponentManager::AddComponent(const std::type_index& aTypeIndex, const int aObjectID)
{
	assert(aObjectID >= 0 && "Tried to add a component to an invalid object ID");
	if (aObjectID < 0)
	{
		return nullptr;
	}
	const int infoIndex = FindTypeInfoIndex(aTypeIndex);
	assert(infoIndex >= 0 && infoIndex < myDataInfo.Size() && "Component type not created");
	if (infoIndex < 0 || infoIndex >= myDataInfo.Size())
	{
		return nullptr;
	}

	ComponentTypeDataInfo& info = myDataInfo[infoIndex];
	assert(info.myStartIndex != -1 && "Component type not created");
	if (info.myStartIndex != -1)
	{
		int dataIndex = info.myStartIndex;
		Component* component = nullptr;
		for (int i = 0; i < info.myAmount; i++)
		{
			component = reinterpret_cast<Component*>(&myComponentData[dataIndex]);
			if (component->GetObjectID() == -1)
			{
				component->myObjectID = aObjectID;
				component->myActive = true;
				component->RecieveMessage(ComponentMessage::Add);
				myObjectComponents[aObjectID].Add(ComponentInfo(info.myTypeIndex, dataIndex));
				#ifndef _RETAIL
				int size = GetAllAddedComponents(aTypeIndex).Size();
				if (size > myMaxComponentCountList[infoIndex])
				{
					myMaxComponentCountList[infoIndex] = size;
				}
				#endif
				return component;
			}
			dataIndex += info.myTypeSize;
		}
		//PRINT_ERROR(std::string("No unused component found of type: %s") + aTypeIndex.name());
		assert(false && "No unused component found.");
	}
	return nullptr;
}

const CU::GrowingArray<Component*> ComponentManager::GetAllAddedComponents(const int aObjectID)
{
	CU::GrowingArray<Component*> returnArray(2);
	if (aObjectID < 0)
	{
		return returnArray;
	}

	const CU::GrowingArray<ComponentInfo, int>& objectComponents = myObjectComponents[aObjectID];
	for (int index = 0; index < objectComponents.Size(); index++)
	{
		returnArray.Add(reinterpret_cast<Component*>(&myComponentData[objectComponents[index].myComponentID]));
	}
	return returnArray;
}

const CU::GrowingArray<Component*> ComponentManager::GetAllAddedComponents(const std::type_index& aTypeIndex)
{
	const int infoIndex = FindTypeInfoIndex(aTypeIndex);
	if (infoIndex < 0 || infoIndex >= myDataInfo.Size())
	{
		return CU::GrowingArray<Component*>(1);
	}

	CU::GrowingArray<Component*> returnArray(2);
	ComponentTypeDataInfo info = myDataInfo[infoIndex];
	assert(info.myStartIndex != -1 && "Component type not created");
	if (info.myStartIndex != -1)
	{
		int dataIndex = 0;
		char* data = &myComponentData[info.myStartIndex];
		Component* component = nullptr;
		for (int compIndex = 0; compIndex < info.myAmount; compIndex++)
		{
			dataIndex = compIndex * info.myTypeSize;
			component = reinterpret_cast<Component*>(&data[dataIndex]);
			if (component->GetObjectID() != -1)
			{
				returnArray.Add(component);
			}
		}
	}
	return returnArray;
}

void ComponentManager::ReleaseComponent(Component* aComponent)
{
	if (aComponent != nullptr && aComponent->GetObjectID() != -1)
	{
		aComponent->RecieveMessage(ComponentMessage::Release);
		aComponent->myActive = false;
		
		CU::GrowingArray<ComponentInfo, int>& objectComponents = myObjectComponents[aComponent->myObjectID];
		aComponent->myObjectID = -1;
		for (int index = 0; index < objectComponents.Size(); index++)
		{
			if (objectComponents[index].myComponentID == static_cast<int>(reinterpret_cast<char*>(aComponent) - myComponentData))
			{
				objectComponents.RemoveCyclicAtIndex(index);
				break;
			}
		}
	}
}

void ComponentManager::ReleaseComponent(const int aComponentID)
{
	assert(aComponentID >= 0 && "Trying to release invalid component ID");
	if (aComponentID < 0)
	{
		return;
	}
	Component* component = reinterpret_cast<Component*>(&myComponentData[aComponentID]);
	ReleaseComponent(component);
}

void ComponentManager::ReleaseObject(const int aObjectID)
{
	assert(aObjectID >= 0 && "Trying to release invalid object ID");
	if (aObjectID < 0)
	{
		return;
	}

	myActiveObjects[aObjectID] = false;
	Component* component = nullptr;
	CU::GrowingArray<ComponentInfo, int>& objectComponents = myObjectComponents[aObjectID];
	for (int index = 0; index < objectComponents.Size(); index++)
	{
		component = reinterpret_cast<Component*>(&myComponentData[objectComponents[index].myComponentID]);

		component->RecieveMessage(ComponentMessage::Release);
		component->myActive = false;
		component->myObjectID = -1;
	}
	myObjectComponents[aObjectID].RemoveAll();
	if (aObjectID >= static_cast<int>(myConstantObjectAmount))
	{
		myObjectIDs.Add(aObjectID);
	}
}

void ComponentManager::ReleaseAllObjects()
{
	Component* component = nullptr;
	char* data = nullptr;
	int dataIndex = 0;
	for (int infoIndex = 0; infoIndex < myDataInfo.Size(); infoIndex++)
	{
		ComponentTypeDataInfo& info = myDataInfo[infoIndex];
		dataIndex = 0;
		data = &myComponentData[info.myStartIndex];
		for (int compIndex = 0; compIndex < info.myAmount; compIndex++)
		{
			dataIndex = compIndex * info.myTypeSize;
			component = reinterpret_cast<Component*>(&data[dataIndex]);
			component->RecieveMessage(ComponentMessage::Release);
			component->myActive = false;
			component->myObjectID = -1;
		}
	}

	for (int index = 0; index < myObjectComponents.Size(); index++)
	{
		myObjectComponents[index].RemoveAll();
	}

	myActiveObjects.reset();
	myObjectIDs.RemoveAll();
	myNextObjectID = myConstantObjectAmount;
}

void ComponentManager::ActivateComponent(Component* aComponent)
{
	aComponent->SetActive(true);
}
void ComponentManager::ActivateComponent(const int aComponentID)
{
	assert(aComponentID >= 0 && "Trying to activate invalid component ID");
	if (aComponentID < 0)
	{
		return;
	}
	Component* component = reinterpret_cast<Component*>(&myComponentData[aComponentID]);
	ActivateComponent(component);
}
void ComponentManager::ActivateObject(const int aObjectID, const bool aActivateComponents)
{
	assert(aObjectID >= 0 && "Trying to activate invalid object ID");
	if (aObjectID < 0 || myActiveObjects[aObjectID])
	{
		return;
	}

	myActiveObjects[aObjectID] = true;
	Component* component = nullptr;
	const CU::GrowingArray<ComponentInfo, int>& objectComponents = myObjectComponents[aObjectID];
	for (int index = 0; index < objectComponents.Size(); index++)
	{
		component = reinterpret_cast<Component*>(&myComponentData[objectComponents[index].myComponentID]);

		if (component->GetActive())
		{
			component->RecieveMessage(ComponentMessage::ActivateObject);
		}
		if (aActivateComponents)
		{
			component->SetActive(true);
		}
	}
}

void ComponentManager::DeactivateComponent(Component* aComponent)
{
	aComponent->SetActive(false);
}
void ComponentManager::DeactivateComponent(const int aComponentID)
{
	assert(aComponentID >= 0 && "Trying to deactivate invalid component ID");
	if (aComponentID < 0)
	{
		return;
	}
	Component* component = reinterpret_cast<Component*>(&myComponentData[aComponentID]);
	DeactivateComponent(component);
}
void ComponentManager::DeactivateObject(const int aObjectID, const bool aDeactivateComponents)
{
	assert(aObjectID >= 0 && "Trying to deactivate invalid object ID");
	if (aObjectID < 0 || !myActiveObjects[aObjectID])
	{
		return;
	}

	myActiveObjects[aObjectID] = false;
	Component* component = nullptr;
	CU::GrowingArray<ComponentInfo, int>& objectComponents = myObjectComponents[aObjectID];
	for (int index = 0; index < objectComponents.Size(); index++)
	{
		component = reinterpret_cast<Component*>(&myComponentData[objectComponents[index].myComponentID]);

		if (component->GetActive())
		{
			component->RecieveMessage(ComponentMessage::DeactivateObject);
		}
		if (aDeactivateComponents)
		{
			component->SetActive(false);
		}
	}
}

int ComponentManager::GetNewObjectID()
{
	if (myObjectIDs.Size() > 0)
	{
		const int newID = myObjectIDs.GetLast();
		myObjectIDs.RemoveLast();

		return newID;
	}

	assert(myNextObjectID < myActiveObjects.size() && "No ObjectIDs left");
	if (myNextObjectID >= myActiveObjects.size())
	{
		CU::Log("Failed to get new object ID", CU::LogType::Error);
		return -1;
	}
	return static_cast<int>(myNextObjectID++);
}

int ComponentManager::ReserveNewObjectIDs(const unsigned int aAmount)
{
	if (aAmount < 1)
	{
		return -1;
	}
	else if (aAmount == 1)
	{
		return GetNewObjectID();
	}
	
	const int newID = static_cast<int>(myNextObjectID);
	assert(myNextObjectID + aAmount < myActiveObjects.size() && "Not enough ObjectIDs");
	if (myNextObjectID + aAmount > myActiveObjects.size())
	{
		CU::Log("Failed to reserve new object IDs", CU::LogType::Error);
		return -1;
	}
	myNextObjectID += aAmount;

	return newID;
}

void ComponentManager::LockCreation()
{
	myCreationLocked = true;
}

bool ComponentManager::GetObjectActive(const int aObjectID) const
{
	return myActiveObjects[aObjectID];
}

void ComponentManager::LogMaxUsedComponents()
{
	#ifndef _RETAIL
	CU::Log("Component Type Max Amount:");
	//PRINT_DEBUG("Component Type Max Amount:");
	for (int infoIndex = 0; infoIndex < myMaxComponentCountList.Size(); infoIndex++)
	{
		CU::Log(myMaxComponentCountList[infoIndex] + ' ' + myDataInfo[infoIndex].myTypeIndex.name());
		//PRINT_DEBUG(myMaxComponentCountList[infoIndex] << ' ' << myDataInfo[infoIndex].myTypeIndex.name());
	}
	#endif
}

void ComponentManager::LogMaxUsedComponents(const std::type_index& aTypeIndex)
{
	aTypeIndex;
	#ifndef _RETAIL
	const int infoIndex = FindTypeInfoIndex(aTypeIndex);
	CU::Log(myMaxComponentCountList[infoIndex] + ' ' + myDataInfo[infoIndex].myTypeIndex.name());
	//PRINT_DEBUG(myMaxComponentCountList[infoIndex] << ' ' << myDataInfo[infoIndex].myTypeIndex.name());
	#endif
}

int ComponentManager::FindTypeInfoIndex(const std::type_index& aTypeIndex)
{
	for (int infoIndex = 0; infoIndex < myDataInfo.Size(); infoIndex++)
	{
		if (myDataInfo[infoIndex].myTypeIndex == aTypeIndex)
		{
			return infoIndex;
		}
	}
	return -1;
}
