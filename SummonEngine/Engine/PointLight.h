#pragma once
#include "Vector4.hpp"
#include <HashMap.hpp>

class PointLight
{
public:
	const CU::Vector3f& GetColor() const;
	const float GetRange() const;

private:
	friend class LightLoader;
	friend class PointLightInstance;
	friend class CU::HashMap<std::string, PointLight>;
	PointLight();
	~PointLight() = default;

	//void AddUser();
	//void RemoveUser();
	//unsigned int GetUserCount();

	CU::Vector3f myColor;
	float myRange;
	//unsigned int myUserCount;
};