#pragma once
#include "ComponentManager.h"
#include <GrowingArray.hpp>

class Component;

class ComponentManagerProxy
{
public:
	ComponentManagerProxy(ComponentManager* aManager);
	~ComponentManagerProxy() = default;

	template<class T>
	T* AddComponent(const int aObjectID);

	template<class T>
	T* GetComponent(const int aObjectID);
	template<class T>
	const T* GetComponent(const int aObjectID) const;
	template<class T>
	CU::GrowingArray<T*> GetComponents(const int aObjectID);

	void ReleaseComponent(Component* aComponent);
	void ReleaseComponent(const int aComponentID);
	void ReleaseObject(const int aObjectID);
	void ActivateComponent(Component* aComponent);
	void ActivateComponent(const int aComponentID);
	void ActivateObject(const int aObjectID);
	void DeactivateComponent(Component* aComponent);
	void DeactivateComponent(const int aComponentID);
	void DeactivateObject(const int aObjectID);

	const int GetNewObjectID();
	const bool GetObjectActive(const int aObjectID) const;

	void LogMaxUsedComponents();
	template<class T>
	void LogMaxUsedComponents();
	void LogMaxUsedComponents(const std::type_index& aTypeIndex);

private:
	ComponentManager* myManager;
};

template<class T>
T* ComponentManagerProxy::AddComponent(const int aObjectID)
{
	return myManager->AddComponent<T>(aObjectID);//maybe be able to send in type_index to manager?
}

template<class T>
T* ComponentManagerProxy::GetComponent(const int aObjectID)
{
	return myManager->GetComponent<T>(aObjectID);
}
template<class T>
inline const T* ComponentManagerProxy::GetComponent(const int aObjectID) const
{
	return myManager->GetComponent<T>(aObjectID);
}
template<class T>
CU::GrowingArray<T*> ComponentManagerProxy::GetComponents(const int aObjectID)
{
	return myManager->GetComponents<T>(aObjectID);
}

template<class T>
inline void ComponentManagerProxy::LogMaxUsedComponents()
{
	#ifndef _RETAIL
	myManager->LogMaxUsedComponents<T>();
	#endif
}