#pragma once
#include <GrowingArray.hpp>
#include <typeinfo>
#include <typeindex>

class Prefab
{
public:
	Prefab();
	~Prefab();

public:
	struct PrefabComponent
	{
		PrefabComponent(const std::type_index& aTypeIndex = typeid(int), char* aData = nullptr) : TypeIndex(aTypeIndex)
		{
			Data = aData;
		}
		std::type_index TypeIndex;
		char* Data;
	};
public:
	CU::GrowingArray<PrefabComponent> myComponents;
};