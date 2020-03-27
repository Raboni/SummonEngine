#pragma once
#include "FontClass.h"
#include <HashMap.hpp>

struct TextData
{
	ID3D11Buffer* myVertexBuffer = nullptr;
	ID3D11Buffer* myIndexBuffer = nullptr;
	CU::Vector2f mySize;
	int myVertexCount = -1;
	int myIndexCount = -1;
};

struct FontVertexData
{
	CU::Vector4f myPosition;
	CU::Vector2f myUV;
};

class Text
{
private:
	friend class TextLoader;
	friend class TextInstance;
	friend class TextRenderer;
	friend class CU::HashMap<std::string, Text>;
	Text();
	~Text();

	void AddUser();
	void RemoveUser();
	const unsigned int GetUserCount() const;

private:
	FontClass myFontObject;
	TextData myData;
	std::string myText;
	std::string myFont;
	unsigned int myUserCount;
};

