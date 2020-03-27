#pragma once
#include "Text.h"
#include "TextInstance.h"
#include "FontClass.h"
#include <HashMap.hpp>
#include <string>

struct ID3D11Device;

class TextLoader
{
public:
	static void CreateInstance(ID3D11Device* aDevice, const unsigned int aMaxTextAmount);
	static TextLoader* GetInstance();
	static void DestroyInstance();

	Text* GetText(const std::string& aText, const std::string& aFont = "font");
	void UnloadText(const std::string& aText, const std::string& aFont);

private:
	TextLoader(ID3D11Device* aDevice, const unsigned int aMaxTextAmount);
	~TextLoader();

	Text* LoadText(const std::string& aText, const std::string& aFont);
	void BuildVertexArray(const std::string& aText, const FontData* aFontData, FontVertexData* aOutVertices, CU::Vector2f& aOutTextSize);

private:
	static TextLoader* myInstance;

	CU::HashMap<std::string, Text> myTextMap;
	ID3D11Device* myDevice;
};

