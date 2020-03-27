#pragma once
#include <HashMap.hpp>
#include <string>

struct ID3D11VertexShader;
struct ID3D11InputLayout;

class VertexShader
{
public:
	ID3D11VertexShader* GetShader();
	ID3D11InputLayout* GetLayout();

private:
	friend class Engine;
	friend class Model;
	friend class ShaderLoader;
	friend class CU::HashMap<std::string, VertexShader>;

	VertexShader();
	~VertexShader() = default;

	void AddUser();
	void RemoveUser();
	unsigned int GetUserCount() const;

private:
	ID3D11VertexShader* myShader;
	ID3D11InputLayout* myLayout;
	unsigned int myUserCount;
};