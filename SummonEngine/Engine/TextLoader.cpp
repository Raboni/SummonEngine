#include "stdafx.h"
#include "TextLoader.h"
#include <d3d11.h>

TextLoader* TextLoader::myInstance = nullptr;
void TextLoader::CreateInstance(ID3D11Device* aDevice, const unsigned int aMaxTextAmount)
{
	if (myInstance == nullptr)
	{
		myInstance = new TextLoader(aDevice, aMaxTextAmount);
	}
}
TextLoader* TextLoader::GetInstance()
{
	return myInstance;
}
void TextLoader::DestroyInstance()
{
	if (myInstance != nullptr)
	{
		delete myInstance;
		myInstance = nullptr;
	}
}

Text* TextLoader::GetText(const std::string& aText, const std::string& aFont)
{
	if (aText.length() <= 0)
	{
		return nullptr;
	}
	std::string hashString = aText + aFont;
	Text* text = myTextMap.Get(hashString);
	if (text == nullptr)
	{
		text = LoadText(aText, aFont);
	}
	text->AddUser();
	return text;
}

void TextLoader::UnloadText(const std::string& aText, const std::string& aFont)
{
	if (aText.length() <= 0)
	{
		return;
	}
	std::string hashString = aText + aFont;
	Text* text = myTextMap.Get(hashString);
	assert(text != nullptr && "Could not find sprite to unload");
	if (text == nullptr)
	{
		return;
	}

	myTextMap.Remove(hashString);

	text->myData.myVertexBuffer->Release();
	text->myData.myIndexBuffer->Release();
	text->myFontObject.myTexture.Release();
}

TextLoader::TextLoader(ID3D11Device* aDevice, const unsigned int aMaxTextAmount) : myTextMap(aMaxTextAmount)
{
	myDevice = aDevice;
}
TextLoader::~TextLoader()
{
}

Text* TextLoader::LoadText(const std::string& aText, const std::string& aFont)
{
	std::string hashString = aText + aFont;
	HRESULT result;
	if (!myTextMap.Create(hashString))
	{
		assert(false && "TextLoader couldn't create text in map");
		return nullptr;
	}
	Text* text = myTextMap.Get(hashString);
	assert(text != nullptr && "TextLoader couldn't find created text");
	if (text == nullptr)
	{
		return text;
	}

	text->myFontObject.Init(aFont);

	unsigned int vertexCount = static_cast<unsigned int>(aText.length()) * 6;
	FontVertexData* vertices = new FontVertexData[vertexCount];
	CU::Vector2f textSize;
	BuildVertexArray(aText, text->myFontObject.myData, vertices, textSize);
	unsigned int* indices = new unsigned int[vertexCount];
	for (unsigned int index = 0; index < vertexCount; index++)
	{
		indices[index] = index;
	}

	#pragma region Buffer
	D3D11_BUFFER_DESC bufferDescription = { 0 };
	bufferDescription.ByteWidth = sizeof(FontVertexData) * vertexCount;
	bufferDescription.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	bufferDescription.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	bufferDescription.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA vertexData = { 0 };
	vertexData.pSysMem = vertices;

	ID3D11Buffer* vertexBuffer;
	result = myDevice->CreateBuffer(&bufferDescription, &vertexData, &vertexBuffer);
	assert(!FAILED(result) && "Failed to create text vertex buffer");
	if (FAILED(result))
	{
		return nullptr;
	}

	bufferDescription.ByteWidth = sizeof(unsigned int) * vertexCount;
	bufferDescription.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	bufferDescription.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	bufferDescription.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA indexData = { 0 };
	indexData.pSysMem = indices;

	ID3D11Buffer* indexBuffer;
	result = myDevice->CreateBuffer(&bufferDescription, &indexData, &indexBuffer);
	assert(!FAILED(result) && "Failed to create text index buffer");
	if (FAILED(result))
	{
		return nullptr;
	}
	delete[] indices;
	indices = nullptr;
	#pragma endregion

	TextData& data = text->myData;
	data.myVertexBuffer = vertexBuffer;
	data.myIndexBuffer = indexBuffer;
	data.mySize = textSize;
	data.myVertexCount = vertexCount;
	data.myIndexCount = vertexCount;
	text->myText = aText;
	text->myFont = aFont;
	text->myUserCount = 0;
	return text;
}

void TextLoader::BuildVertexArray(const std::string& aText, const FontData* aFontData, FontVertexData* aOutVertices, CU::Vector2f& aOutTextSize)
{
	aOutTextSize = CU::Vector2f::Zero;
	CU::Vector2f drawPosition = CU::Vector2f::Zero;
	const float spaceDistance = 3.0f;
	const float newLineDistance = 16.0f;
	const float bottomUV = 0.8125f;
	int numLetters = static_cast<int>(aText.length());
	int index = 0;
	int letter = -1;
	// Draw each letter onto a quad.
	for (int i = 0; i < numLetters; i++)
	{
		letter = static_cast<int>(aText[i]) - 32;

		// If the letter is a space then just move over three pixels.
		if (aText[i] == ' ')
		{
			drawPosition.x += spaceDistance;
		}
		else if (aText[i] == '\n')
		{
			if (drawPosition.x > aOutTextSize.x)
			{
				aOutTextSize.x = drawPosition.x;
			}
			drawPosition.x = 0.0f;
			drawPosition.y -= newLineDistance;
		}
		else
		{
			// First triangle in quad.
			aOutVertices[index].myPosition = CU::Vector4f(drawPosition.x, drawPosition.y, 0.0f, 1.0f);  // Top left.
			aOutVertices[index].myUV = CU::Vector2f(aFontData[letter].myLeft, 0.0f);
			index++;

			aOutVertices[index].myPosition = CU::Vector4f((drawPosition.x + aFontData[letter].mySize), (drawPosition.y - 16), 0.0f, 1.0f);  // Bottom right.
			aOutVertices[index].myUV = CU::Vector2f(aFontData[letter].myRight, bottomUV);
			index++;

			aOutVertices[index].myPosition = CU::Vector4f(drawPosition.x, (drawPosition.y - 16), 0.0f, 1.0f);  // Bottom left.
			aOutVertices[index].myUV = CU::Vector2f(aFontData[letter].myLeft, bottomUV);
			index++;

			// Second triangle in quad.
			aOutVertices[index].myPosition = CU::Vector4f(drawPosition.x, drawPosition.y, 0.0f, 1.0f);  // Top left.
			aOutVertices[index].myUV = CU::Vector2f(aFontData[letter].myLeft, 0.0f);
			index++;

			aOutVertices[index].myPosition = CU::Vector4f(drawPosition.x + aFontData[letter].mySize, drawPosition.y, 0.0f, 1.0f);  // Top right.
			aOutVertices[index].myUV = CU::Vector2f(aFontData[letter].myRight, 0.0f);
			index++;

			aOutVertices[index].myPosition = CU::Vector4f((drawPosition.x + aFontData[letter].mySize), (drawPosition.y - 16), 0.0f, 1.0f);  // Bottom right.
			aOutVertices[index].myUV = CU::Vector2f(aFontData[letter].myRight, bottomUV);
			index++;

			// Update the x location for drawing by the size of the letter and one pixel.
			drawPosition.x = drawPosition.x + aFontData[letter].mySize + 1.0f;
		}
	}
	drawPosition.y -= newLineDistance;
	if (drawPosition.x > aOutTextSize.x)
	{
		aOutTextSize.x = drawPosition.x;
	}
	aOutTextSize.y = drawPosition.y;
}