#pragma once
#include "Component.h"

class MetaTagComponent : public Component
{
public:
	MetaTagComponent();
	MetaTagComponent(int aComponentID, int aObjectID, ComponentManager* aManager);
	~MetaTagComponent();

	void SetTag(size_t aTag);
	const size_t GetTag() const;

protected:
	void OnAdd() override;

private:
	size_t myTag;
};
