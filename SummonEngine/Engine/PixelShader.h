#pragma once
#include <HashMap.hpp>
#include <string>

struct ID3D11PixelShader;

class PixelShader
{
public:
	ID3D11PixelShader* GetShader();

private:
	friend class Engine;
	friend class Model;
	friend class ShaderLoader;
	friend class CU::HashMap<std::string, PixelShader>;

	PixelShader();
	~PixelShader() = default;

	void AddUser();
	void RemoveUser();
	unsigned int GetUserCount() const;

private:
	ID3D11PixelShader* myShader;
	unsigned int myUserCount;
};