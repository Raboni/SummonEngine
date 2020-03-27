#pragma once
#include <HashMap.hpp>

struct ID3D11ShaderResourceView;
class TextureLoader;

class Texture
{
public:
	ID3D11ShaderResourceView** GetSRV();

private:
	friend class TextureLoader;
	friend class TextureInstance;
	friend class CU::HashMap<std::string, Texture>;
	Texture();
	~Texture();

	void AddUser();
	void RemoveUser();
	unsigned int GetUserCount();

	unsigned int myUserCount;
	ID3D11ShaderResourceView* myTexture;
};