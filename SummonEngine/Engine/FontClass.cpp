#include "stdafx.h"
#include "FontClass.h"
#include <fstream>

std::string FontClass::ourFontPath = "Fonts\\";

FontClass::FontClass()
{
}
FontClass::~FontClass()
{
}

const bool FontClass::Init(const std::string& aFontName)
{
	std::string path = ourFontPath + aFontName;

	bool result;
	// Load in the text file containing the font data.
	result = LoadFontData(path + ".txt");
	if (!result)
	{
		return false;
	}

	myTexture.Init(path + ".dds");

	return true;
}

const bool FontClass::LoadFontData(const std::string& aPath)
{
	std::ifstream fin;
	char temp;

	// Read in the font size and spacing between chars.
	fin.open(aPath);
	if (fin.fail())
	{
		return false;
	}

	// Read in the 95 used ascii characters for text.
	for (int i = 0; i < ourFontMaxCharacterCount; i++)
	{
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}

		fin >> myData[i].myLeft;
		fin >> myData[i].myRight;
		fin >> myData[i].mySize;
	}

	// Close the file.
	fin.close();

	return true;
}

ID3D11ShaderResourceView** FontClass::GetTexture()
{
	return myTexture.GetTexture()->GetSRV();
}

/*void FontClass::BuildVertexArray(void* aVertices, const std::string& aSentence, float aDrawX, float aDrawY)
{
	VertexType* vertexPtr;
	int numLetters, index, i, letter;


	// Coerce the input vertices into a VertexType structure.
	vertexPtr = (VertexType*)aVertices;

	// Get the number of letters in the sentence.
	numLetters = (int)aSentence.length();

	// Initialize the index to the vertex array.
	index = 0;
	// Draw each letter onto a quad.
	for (i = 0; i < numLetters; i++)
	{
		letter = ((int)aSentence[i]) - 32;

		// If the letter is a space then just move over three pixels.
		if (letter == 0)
		{
			aDrawX = aDrawX + 3.0f;
		}
		else
		{
			// First triangle in quad.
			vertexPtr[index].myPosition = CU::Vector3f(aDrawX, aDrawY, 0.0f);  // Top left.
			vertexPtr[index].myTexture = CU::Vector2f(myFont[letter].myLeft, 0.0f);
			index++;

			vertexPtr[index].myPosition = CU::Vector3f((aDrawX + myFont[letter].mySize), (aDrawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].myTexture = CU::Vector2f(myFont[letter].myRight, 1.0f);
			index++;

			vertexPtr[index].myPosition = CU::Vector3f(aDrawX, (aDrawY - 16), 0.0f);  // Bottom left.
			vertexPtr[index].myTexture = CU::Vector2f(myFont[letter].myLeft, 1.0f);
			index++;

			// Second triangle in quad.
			vertexPtr[index].myPosition = CU::Vector3f(aDrawX, aDrawY, 0.0f);  // Top left.
			vertexPtr[index].myTexture = CU::Vector2f(myFont[letter].myLeft, 0.0f);
			index++;

			vertexPtr[index].myPosition = CU::Vector3f(aDrawX + myFont[letter].mySize, aDrawY, 0.0f);  // Top right.
			vertexPtr[index].myTexture = CU::Vector2f(myFont[letter].myRight, 0.0f);
			index++;

			vertexPtr[index].myPosition = CU::Vector3f((aDrawX + myFont[letter].mySize), (aDrawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].myTexture = CU::Vector2f(myFont[letter].myRight, 1.0f);
			index++;

			// Update the x location for drawing by the size of the letter and one pixel.
			aDrawX = aDrawX + myFont[letter].mySize + 1.0f;
		}
	}
}*/