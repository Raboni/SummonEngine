#pragma once
#include "../ComponentManagerProxy.h"
#include "../ComponentMessage.h"

class Component
{
public:
	Component() = delete;
	Component(const int aID, const int aObjectID, ComponentManager* aManager);
	virtual ~Component() = default;

	virtual void PrefabInit(const char* aData);
	virtual void PrefabCustomAction(const char aActionID, const char* aData);
	virtual void Update(const float aDeltaTime);
	
	//Set whether the component should be active or not
	void SetActive(const bool aActive);
	//Get whether the component is active or not
	bool GetActive() const;

	//Get the ObjectID this component is attached to
	int GetObjectID() const;
	//Get the ComponentID of this component
	int GetComponentID() const;

protected:
	friend class ComponentManager;
	friend class ComponentPostMaster;

	void RecieveMessage(const ComponentMessage aMessage);
	virtual void RecieveForwardMessage(const ComponentMessage aMessage);

	virtual void OnAdd() = 0;
	virtual void OnActivate();
	virtual void OnDeactivate();
	virtual void OnRelease();

	//Release this component from its object
	void Release();

	//Get first component of type T from the same ObjectID as this component
	template <class T>
	T* GetComponent();
	//Get first component of type T from aObjectID
	template <class T>
	T* GetComponent(const int aObjectID);

	//Add a component of type T to the same ObjectID as this component
	template <class T>
	T* AddComponent();
	//Add a component of type T to aObjectID
	template <class T>
	T* AddComponent(const int aObjectID);

	int GetNewObjectID();

protected:
	ComponentManagerProxy myManager;
	
private:
	int myObjectID;
	int myID;
	bool myActive;
};

template<class T>
inline T* Component::GetComponent()
{
	return myManager.GetComponent<T>(GetObjectID());
}
template<class T>
inline T* Component::GetComponent(const int aObjectID)
{
	return myManager.GetComponent<T>(aObjectID);
}
template <class T>
inline T* Component::AddComponent()
{
	return myManager.AddComponent<T>(GetObjectID());
}
template <class T>
inline T* Component::AddComponent(const int aObjectID)
{
	return myManager.AddComponent<T>(aObjectID);
}
