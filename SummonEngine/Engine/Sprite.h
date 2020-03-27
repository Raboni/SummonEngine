#pragma once
#include <HashMap.hpp>

struct ID3D11ShaderResourceView;
struct ID3D11VertexShader;
struct ID3D11GeometryShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
class SpriteLoader;

struct SpriteData
{
	ID3D11VertexShader* myVertexShader = nullptr;
	ID3D11GeometryShader* myGeometryShader = nullptr;
	ID3D11PixelShader* myPixelShader = nullptr;
	ID3D11InputLayout* myInputLayout = nullptr;
	unsigned int myStride = 0;
	unsigned int myOffset = 0;
	unsigned short myTopology = 0;
};

class Sprite
{
public:
	ID3D11ShaderResourceView** GetSRV();
	const SpriteData& GetData() const;

private:
	friend class Engine;
	friend class Scene;
	friend class SpriteLoader;
	friend class SpriteInstance;
	friend class CU::HashMap<std::string, Sprite>;
	Sprite();
	~Sprite();

	void AddUser();
	void RemoveUser();
	const unsigned int GetUserCount() const;

private:
	SpriteData myData;
	ID3D11ShaderResourceView* myTexture;
	unsigned int myUserCount;
};