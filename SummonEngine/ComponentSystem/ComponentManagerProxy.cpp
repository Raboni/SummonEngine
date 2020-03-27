#include "Components/stdafx.h"
#include "ComponentManagerProxy.h"
#include "ComponentManager.h"

ComponentManagerProxy::ComponentManagerProxy(ComponentManager* aManager)
{
	myManager = aManager;
}

void ComponentManagerProxy::ReleaseComponent(Component* aComponent)
{
	myManager->ReleaseComponent(aComponent);
}
void ComponentManagerProxy::ReleaseComponent(const int aComponentID)
{
	myManager->ReleaseComponent(aComponentID);
}
void ComponentManagerProxy::ReleaseObject(const int aObjectID)
{
	myManager->ReleaseObject(aObjectID);
}

void ComponentManagerProxy::ActivateComponent(Component* aComponent)
{
	myManager->ActivateComponent(aComponent);
}
void ComponentManagerProxy::ActivateComponent(const int aComponentID)
{
	myManager->ActivateComponent(aComponentID);
}
void ComponentManagerProxy::ActivateObject(const int aObjectID)
{
	myManager->ActivateObject(aObjectID);
}

void ComponentManagerProxy::DeactivateComponent(Component* aComponent)
{
	myManager->DeactivateComponent(aComponent);
}
void ComponentManagerProxy::DeactivateComponent(const int aComponentID)
{
	myManager->DeactivateComponent(aComponentID);
}
void ComponentManagerProxy::DeactivateObject(const int aObjectID)
{
	myManager->DeactivateObject(aObjectID);
}

const int ComponentManagerProxy::GetNewObjectID()
{
	return myManager->GetNewObjectID();
}

const bool ComponentManagerProxy::GetObjectActive(const int aObjectID) const
{
	return myManager->GetObjectActive(aObjectID);
}

void ComponentManagerProxy::LogMaxUsedComponents()
{
	#ifndef _RETAIL
	myManager->LogMaxUsedComponents();
	#endif
}
void ComponentManagerProxy::LogMaxUsedComponents(const std::type_index& aTypeIndex)
{
	aTypeIndex;
	#ifndef _RETAIL
	myManager->LogMaxUsedComponents(aTypeIndex);
	#endif
}