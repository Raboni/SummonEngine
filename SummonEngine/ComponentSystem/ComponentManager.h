#pragma once
#include "ComponentMessage.h"
#include <GrowingArray.hpp>
#include <DebugTools.h>
#include <typeinfo>
#include <typeindex>
#include <assert.h>
#include <bitset>

class Component;

constexpr size_t ourMaxObjectAmount = 8 * sizeof(unsigned long long) * 800;

struct ComponentTypeDataInfo
{
	ComponentTypeDataInfo() : myTypeIndex(typeid(ComponentTypeDataInfo))
	{
		myStartIndex = -1;
		myAmount = 0;
		myTypeSize = 0;
	}
	std::type_index myTypeIndex;
	int myStartIndex;
	unsigned short myAmount;
	unsigned short myTypeSize;
	bool myShouldUpdate;
};
struct ComponentInfo
{
	ComponentInfo() : myTypeIndex(typeid(ComponentInfo))
	{
		myComponentID = -1;
	}
	ComponentInfo(const std::type_index& aTypeIndex, const int aComponentID) : myTypeIndex(aTypeIndex)
	{
		myComponentID = aComponentID;
	}
	std::type_index myTypeIndex;
	int myComponentID;
};

class ComponentManager
{
public:
	ComponentManager();
	~ComponentManager();

	void Init(const unsigned int aObjectAmount, const unsigned int aConstantObjectAmount = 0);
	void Update(const float aDeltaTime);

	template<class T>
	T* AddComponent(const int aObjectID);
	Component* AddComponent(const std::type_index& aTypeIndex, const int aObjectID);

	template<class T>
	T* GetComponent(const int aObjectID);
	template<class T>
	const T* GetComponent(const int aObjectID) const;
	template<class T>
	const CU::GrowingArray<T*> GetComponents(const int aObjectID);
	template<class T>
	const CU::GrowingArray<T*> GetAllAddedComponents();
	const CU::GrowingArray<Component*> GetAllAddedComponents(const int aObjectID);
	const CU::GrowingArray<Component*> GetAllAddedComponents(const std::type_index& aTypeIndex);

	void ReleaseComponent(Component* aComponent);
	void ReleaseComponent(const int aComponentID);
	void ReleaseObject(const int aObjectID);
	void ReleaseAllObjects();
	void ActivateComponent(Component* aComponent);
	void ActivateComponent(const int aComponentID);
	void ActivateObject(const int aObjectID, const bool aActivateComponents = false);
	void DeactivateComponent(Component* aComponent);
	void DeactivateComponent(const int aComponentID);
	void DeactivateObject(const int aObjectID, const bool aDeactivateComponents = false);

	template<class T>
	void CreateType(const unsigned short aAmount, const bool aShouldUpdate = true);
	int GetNewObjectID();
	//returns the first ID, all reserved IDs are sequential
	int ReserveNewObjectIDs(const unsigned int aAmount);
	void LockCreation();
	bool GetObjectActive(const int aObjectID) const;

	void LogMaxUsedComponents();
	template<class T>
	void LogMaxUsedComponents();
	void LogMaxUsedComponents(const std::type_index& aTypeIndex);

private:
	template<class T>
	int FindTypeInfoIndex();
	int FindTypeInfoIndex(const std::type_index& aTypeIndex);

private:
	std::bitset<ourMaxObjectAmount> myActiveObjects;
	CU::GrowingArray<ComponentTypeDataInfo, int> myDataInfo;
	CU::GrowingArray<int, int> myObjectIDs;
	#ifndef _RETAIL
	CU::GrowingArray<int, int> myMaxComponentCountList;
	#endif
	CU::GrowingArray<CU::GrowingArray<ComponentInfo, int>, int> myObjectComponents;
	char* myComponentData;
	unsigned int myDataSize;
	unsigned int myLastDataIndex;
	unsigned int myNextObjectID;
	unsigned int myConstantObjectAmount;
	bool myCreationLocked;
};

template<class T>
inline T* ComponentManager::AddComponent(const int aObjectID)
{
	static_assert(std::is_base_of<Component, T>::value, "Template type is not a component");

	assert(aObjectID >= 0 && "Tried to add a component to an invalid object ID");
	if (aObjectID < 0)
	{
		return nullptr;
	}
	const int infoIndex = FindTypeInfoIndex<T>();
	assert(infoIndex >= 0 && infoIndex < myDataInfo.Size() && "Component type not created");
	if (infoIndex < 0 || infoIndex >= myDataInfo.Size())
	{
		return nullptr;
	}

	const ComponentTypeDataInfo& info = myDataInfo[infoIndex];
	assert(info.myStartIndex != -1 && "Component type not created");
	if (info.myStartIndex != -1)
	{
		int dataIndex = info.myStartIndex;
		T* component = nullptr;
		for (int i = 0; i < info.myAmount; i++)
		{
			component = reinterpret_cast<T*>(&myComponentData[dataIndex]);
			if (component->GetObjectID() == -1)
			{
				component->myObjectID = aObjectID;
				component->myActive = true;
				component->RecieveMessage(ComponentMessage::Add);
				myObjectComponents[aObjectID].Add(ComponentInfo(info.myTypeIndex, dataIndex));

				#ifndef _RETAIL
				const int size = GetAllAddedComponents<T>().Size();
				if (size > myMaxComponentCountList[infoIndex])
				{
					myMaxComponentCountList[infoIndex] = size;
				}
				#endif

				return component;
			}
			dataIndex += info.myTypeSize;
		}
		CU::Log(std::string("No unused component found of type: %s") + typeid(T).name(), CU::LogType::Error);
		assert(false && "No unused component found");
	}
	return nullptr;
}

template<class T>
inline T* ComponentManager::GetComponent(const int aObjectID)
{
	static_assert(std::is_base_of<Component, T>::value, "Template type is not a component");

	assert(aObjectID >= 0 && "Tried to get component of invalid object ID");
	if (aObjectID < 0)
	{
		CU::Log("Tried to get component of invalid object ID", CU::LogType::Error);
		return nullptr;
	}

	const std::type_index findIndex(typeid(T));
	const CU::GrowingArray<ComponentInfo, int>& objectComponents = myObjectComponents[aObjectID];
	for (int index = 0; index < objectComponents.Size(); index++)
	{
		if (objectComponents[index].myTypeIndex == findIndex)
		{
			return reinterpret_cast<T*>(&myComponentData[objectComponents[index].myComponentID]);
		}
	}
	return nullptr;
}
template<class T>
inline const T* ComponentManager::GetComponent(const int aObjectID) const
{
	static_assert(std::is_base_of<Component, T>::value, "Template type is not a component");

	assert(aObjectID >= 0 && "Tried to get component of invalid object ID");
	if (aObjectID < 0)
	{
		CU::Log("Tried to get component of invalid object ID", CU::LogType::Error);
		return nullptr;
	}

	const std::type_index findIndex(typeid(T));
	const CU::GrowingArray<ComponentInfo, int>& objectComponents = myObjectComponents[aObjectID];
	for (int index = 0; index < objectComponents.Size(); index++)
	{
		if (objectComponents[index].myTypeIndex == findIndex)
		{
			return reinterpret_cast<T*>(&myComponentData[objectComponents[index].myComponentID]);
		}
	}
	return nullptr;
}
template<class T>
inline const CU::GrowingArray<T*> ComponentManager::GetComponents(const int aObjectID)
{
	static_assert(std::is_base_of<Component, T>::value, "Template type is not a component");

	CU::GrowingArray<T*> returnArray(2);
	assert(aObjectID >= 0 && "Tried to get components of invalid object ID");
	if (aObjectID < 0)
	{
		CU::Log("Tried to get components of invalid object ID", CU::LogType::Error);
		return returnArray;
	}

	const std::type_index findIndex(typeid(T));
	const CU::GrowingArray<ComponentInfo, int>& objectComponents = myObjectComponents[aObjectID];
	for (int index = 0; index < objectComponents.Size(); index++)
	{
		if (objectComponents[index].myTypeIndex == findIndex)
		{
			returnArray.Add(reinterpret_cast<T*>(&myComponentData[objectComponents[index].myComponentID]));
		}
	}
	return returnArray;
}

template<class T>
inline const CU::GrowingArray<T*> ComponentManager::GetAllAddedComponents()
{
	static_assert(std::is_base_of<Component, T>::value, "Template type is not a component");

	CU::GrowingArray<T*> returnArray(2);
	const int infoIndex = FindTypeInfoIndex<T>();
	if (infoIndex < 0 || infoIndex >= myDataInfo.Size())
	{
		return returnArray;
	}

	const ComponentTypeDataInfo& info = myDataInfo[infoIndex];
	assert(info.myStartIndex != -1 && "Component type not created");
	if (info.myStartIndex != -1)
	{
		int dataIndex = 0;
		char* data = &myComponentData[info.myStartIndex];
		T* component = nullptr;
		for (int compIndex = 0; compIndex < info.myAmount; compIndex++)
		{
			dataIndex = compIndex * info.myTypeSize;
			component = reinterpret_cast<T*>(&data[dataIndex]);
			if (component->GetObjectID() != -1)
			{
				returnArray.Add(component);
			}
		}
	}
	return returnArray;
}

template<class T>
inline void ComponentManager::CreateType(const unsigned short aAmount, const bool aShouldUpdate)
{
	static_assert(std::is_base_of<Component, T>::value, "Template type is not a component");

	assert(!myCreationLocked && "Component creation is locked");
	if (myCreationLocked)
	{
		return;
	}

	ComponentTypeDataInfo& info = myDataInfo.Add();
	info.myTypeIndex = std::type_index(typeid(T));
	info.myTypeSize = sizeof(T);
	info.myShouldUpdate = aShouldUpdate;

	for (int infoIndex = 0; infoIndex < myDataInfo.Size() - 1; infoIndex++)
	{
		assert(myDataInfo[infoIndex].myTypeIndex != info.myTypeIndex && "Component type already created");
		if (myDataInfo[infoIndex].myTypeIndex == info.myTypeIndex)
		{
			CU::Log("Tried to create a component type that already exists", CU::LogType::Error);
			myDataInfo.RemoveLast();
			return;
		}
	}

	const int newDataSize = myDataSize + info.myTypeSize * aAmount;
	char* tempData = new char[newDataSize];
	memcpy(tempData, myComponentData, myDataSize);
	delete[] myComponentData;
	myComponentData = tempData;
	myDataSize = newDataSize;

	info.myAmount = aAmount;
	info.myStartIndex = myLastDataIndex;
	for (int i = 0; i < aAmount; i++)
	{
		new(&myComponentData[myLastDataIndex]) T(myLastDataIndex, -1, this);
		myLastDataIndex += info.myTypeSize;
	}
	#ifndef _RETAIL
	myMaxComponentCountList.Add(0);
	#endif
}

template<class T>
inline void ComponentManager::LogMaxUsedComponents()
{
	#ifndef _RETAIL
	const int infoIndex = FindTypeInfoIndex<T>();
	CU::Log(std::to_string(myMaxComponentCountList[infoIndex]) + ' ' + typeid(T).name());
	//PRINT_DEBUG(myMaxComponentCountList[infoIndex] << ' ' << typeid(T).name());
	#endif
}

template<class T>
inline int ComponentManager::FindTypeInfoIndex()
{
	static_assert(std::is_base_of<Component, T>::value, "Template type is not a component");

	const std::type_index typeIndex(typeid(T));
	for (int infoIndex = 0; infoIndex < myDataInfo.Size(); infoIndex++)
	{
		if (myDataInfo[infoIndex].myTypeIndex == typeIndex)
		{
			return infoIndex;
		}
	}
	return -1;
}
