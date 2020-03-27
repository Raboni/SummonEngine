#pragma once
#include <StateStackProxy.hpp>

class ComponentManager;

class Editor
{
public:
	Editor(CU::StateStackProxy aStateStack);
	~Editor() = default;

	void Init(ComponentManager* aComponentManager);
	void Start();

private:
	CU::StateStackProxy myStateStack;
};

