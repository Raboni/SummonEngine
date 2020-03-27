#pragma once
#include <HashMap.hpp>
#include <string>

struct ID3D11GeometryShader;

class GeometryShader
{
public:
	ID3D11GeometryShader* GetShader();

private:
	friend class Engine;
	friend class Model;
	friend class ShaderLoader;
	friend class CU::HashMap<std::string, GeometryShader>;

	GeometryShader();
	~GeometryShader() = default;

	void AddUser();
	void RemoveUser();
	unsigned int GetUserCount() const;

private:
	ID3D11GeometryShader* myShader;
	unsigned int myUserCount;
};