#include "stdafx.h"
#include "TextInstance.h"
#include "TextLoader.h"

static std::string ourErrorString = "";

TextInstance::TextInstance()
{
	myTextObject = nullptr;
	myColor = CU::Vector4f::One;
	myScale = CU::Vector2f::One;
	myPivot = CU::Vector2f(0.0f, 0.5f);
}
TextInstance::TextInstance(const TextInstance& aInstance)
{
	myColor = aInstance.myColor;
	myPosition = aInstance.myPosition;
	myScale = aInstance.myScale;
	myPivot = aInstance.myPivot;
	myTextObject = aInstance.myTextObject;
	if (myTextObject != nullptr)
	{
		myTextObject->AddUser();
	}
}
TextInstance& TextInstance::operator=(const TextInstance& aInstance)
{
	Release();
	myColor = aInstance.myColor;
	myPosition = aInstance.myPosition;
	myScale = aInstance.myScale;
	myPivot = aInstance.myPivot;
	myTextObject = aInstance.myTextObject;
	if (myTextObject != nullptr)
	{
		myTextObject->AddUser();
	}
	return *this;
}
TextInstance::~TextInstance()
{
	Release();
}

void TextInstance::Init(const std::string& aText, const std::string& aFont)
{
	Release();
	myTextObject = TextLoader::GetInstance()->GetText(aText, aFont);
	assert(myTextObject != nullptr && "Failed to create text");
}
void TextInstance::Release()
{
	if (myTextObject != nullptr)
	{
		myTextObject->RemoveUser();
		if (myTextObject->GetUserCount() <= 0)
		{
			TextLoader::GetInstance()->UnloadText(myTextObject->myText, myTextObject->myFont);
		}
		myTextObject = nullptr;
	}
}

void TextInstance::SetColor(const CU::Vector3f& aColor)
{
	myColor = CU::Vector4f(aColor, myColor.w);
}

void TextInstance::SetColor(const CU::Vector4f& aColor)
{
	myColor = aColor;
}
void TextInstance::SetPosition(const CU::Vector3f& aPosition)
{
	myPosition = CU::Vector4f(aPosition, 1.0f);
}
void TextInstance::SetPosition(const CU::Vector2f& aPosition)
{
	myPosition = CU::Vector4f(aPosition.x, aPosition.y, 0.0f, 1.0f);
}
void TextInstance::SetScale(const CU::Vector2f& aScale)
{
	myScale = aScale;
}
void TextInstance::SetPivot(const CU::Vector2f& aPivot)
{
	myPivot = aPivot;
}

const CU::Vector4f& TextInstance::GetColor() const
{
	return myColor;
}
const CU::Vector3f& TextInstance::GetPosition() const
{
	return myPosition;
}
const CU::Vector2f& TextInstance::GetScale() const
{
	return myScale;
}
const CU::Vector2f& TextInstance::GetPivot() const
{
	return myPivot;
}

const CU::Vector2f TextInstance::GetSizeInPixels() const
{
	if (myTextObject == nullptr)
	{
		return CU::Vector2f::Zero;
	}
	return myTextObject->myData.mySize * myScale;
}
const std::string& TextInstance::GetText() const
{
	if (myTextObject == nullptr)
	{
		return ourErrorString;
	}
	return myTextObject->myText;
}
const std::string& TextInstance::GetFont() const
{
	if (myTextObject == nullptr)
	{
		return ourErrorString;
	}
	return myTextObject->myFont;
}