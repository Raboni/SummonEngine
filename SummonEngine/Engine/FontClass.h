#pragma once
#include "TextureInstance.h"
#include <Vector.hpp>
#include <string>

struct ID3D11ShaderResourceView;

constexpr unsigned int ourFontMaxCharacterCount = 95;

struct FontData
{
	float myLeft;
	float myRight;
	int mySize;
};

class FontClass
{
public:
	FontClass();
	~FontClass();

	const bool Init(const std::string& aFontName);

	ID3D11ShaderResourceView** GetTexture();

	//void BuildVertexArray(void* aVertices, const std::string& aSentence, float aDrawX, float aDrawY);

private:
	friend class TextLoader;
	const bool LoadFontData(const std::string& aPath);

private:
	static std::string ourFontPath;

	FontData myData[ourFontMaxCharacterCount];
	TextureInstance myTexture;
};