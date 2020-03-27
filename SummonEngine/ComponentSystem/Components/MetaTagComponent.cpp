#include "stdafx.h"
#include "MetaTagComponent.h"

MetaTagComponent::MetaTagComponent() : Component(-1, -1, nullptr)
{
}

MetaTagComponent::MetaTagComponent(int aComponentID, int aObjectID,
	ComponentManager* aManager) : Component(aComponentID, aObjectID, aManager)
{
}

MetaTagComponent::~MetaTagComponent()
{
}

void MetaTagComponent::SetTag(size_t aTag)
{
	myTag = aTag;
}

const size_t MetaTagComponent::GetTag() const
{
	return myTag;
}

void MetaTagComponent::OnAdd()
{
}