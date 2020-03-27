#include "stdafx.h"
#include "Component.h"
#include "../ComponentManagerProxy.h"

Component::Component(const int aID, const int aObjectID, ComponentManager* aManager) : myManager(aManager)
{
	myID = aID;
	myObjectID = aObjectID;
	myActive = false;
}

void Component::PrefabInit(const char* aData)
{
	aData;
}
void Component::PrefabCustomAction(const char aActionID, const char* aData)
{
	aActionID;
	aData;
}

void Component::Update(const float aDeltaTime)
{
	aDeltaTime;
}

void Component::SetActive(const bool aActive)
{
	if (aActive)
	{
		RecieveMessage(ComponentMessage::Activate);
	}
	else
	{
		RecieveMessage(ComponentMessage::Deactivate);
	}
}
bool Component::GetActive() const
{
	return myActive;
}

int Component::GetObjectID() const
{
	return myObjectID;
}
int Component::GetComponentID() const
{
	return myID;
}

void Component::RecieveMessage(const ComponentMessage aMessage)
{
	switch (aMessage)
	{
	case ComponentMessage::Add:
		RecieveForwardMessage(aMessage);
		OnAdd();
		break;
	case ComponentMessage::Activate:
		if (!myActive)
		{
			myActive = true;
			RecieveForwardMessage(aMessage);
			OnActivate();
		}
		break;
	case ComponentMessage::ActivateObject:
		RecieveForwardMessage(aMessage);
		OnActivate();
		break;
	case ComponentMessage::Deactivate:
		if (myActive)
		{
			myActive = false;
			RecieveForwardMessage(aMessage);
			OnDeactivate();
		}
		break;
	case ComponentMessage::DeactivateObject:
		RecieveForwardMessage(aMessage);
		OnDeactivate();
		break;
	case ComponentMessage::Release:
		if (myObjectID != -1)
		{
			RecieveForwardMessage(aMessage);
			OnRelease();
		}
		break;
	default:
		RecieveForwardMessage(aMessage);
		break;
	}
}

void Component::RecieveForwardMessage(const ComponentMessage aMessage)
{
	aMessage;
}

void Component::OnActivate()
{
}
void Component::OnDeactivate()
{
}
void Component::OnRelease()
{
}

void Component::Release()
{
	myManager.ReleaseComponent(this);
}

int Component::GetNewObjectID()
{
	return myManager.GetNewObjectID();
}