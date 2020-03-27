#include "stdafx.h"
#include "SpriteInstance.h"
#include "SpriteLoader.h"

SpriteInstance::SpriteInstance()
{
	mySprite = nullptr;
	myPath = "";
}
SpriteInstance::SpriteInstance(const std::string& aPath, const CU::Vector2f& aPosition, const CU::Vector2f& aSize)
{
	mySprite = nullptr;
	Init(aPath, aPosition, aSize);
}
SpriteInstance::SpriteInstance(const SpriteInstance& aInstance)
{
	myData = aInstance.myData;
	myPath = aInstance.myPath;
	mySprite = aInstance.mySprite;
	if (mySprite != nullptr)
	{
		mySprite->AddUser();
	}
}
SpriteInstance& SpriteInstance::operator=(const SpriteInstance& aInstance)
{
	Release();
	myData = aInstance.myData;
	myPath = aInstance.myPath;
	mySprite = aInstance.mySprite;
	if (mySprite != nullptr)
	{
		mySprite->AddUser();
	}
	return *this;
}
SpriteInstance::~SpriteInstance()
{
	Release();
}

void SpriteInstance::Init(const std::string& aPath, const CU::Vector2f& aPosition, const CU::Vector2f& aSize)
{
	Release();
	myPath = aPath;
	mySprite = SpriteLoader::GetInstance()->GetSprite(myPath);
	assert(mySprite != nullptr && "Unable to get texture");
	myData.myColor = CU::Vector4f::One;
	myData.myPosition = aPosition;
	myData.mySize = aSize;
	myData.myPivot = CU::Vector2f(0.0f, 0.0f);
	myData.myRotation = 0.0f;
}

void SpriteInstance::Release()
{
	if (mySprite != nullptr)
	{
		mySprite->RemoveUser();
		if (mySprite->GetUserCount() <= 0)
		{
			SpriteLoader::GetInstance()->UnloadSprite(myPath);
		}
		mySprite = nullptr;
	}
}

void SpriteInstance::SetSprite(const std::string& aPath)
{
	Release();
	myPath = aPath;
	mySprite = SpriteLoader::GetInstance()->GetSprite(myPath);
	assert(mySprite != nullptr && "Unable to get texture");
}
void SpriteInstance::SetPosition(const CU::Vector2f& aPosition)
{
	myData.myPosition = aPosition;
}
void SpriteInstance::SetSize(const CU::Vector2f& aSize)
{
	myData.mySize = aSize;
}
void SpriteInstance::SetPivot(const CU::Vector2f& aPivot)
{
	myData.myPivot = aPivot;
}
void SpriteInstance::SetRotation(const float aRotation)
{
	myData.myRotation = aRotation;
}
void SpriteInstance::SetColor(const CU::Vector4f& aColor)
{
	myData.myColor = aColor;
}
void SpriteInstance::SetColor(const CU::Vector3f& aColor)
{
	myData.myColor.x = aColor.x;
	myData.myColor.y = aColor.y;
	myData.myColor.z = aColor.z;
}
void SpriteInstance::SetAlpha(const float aAlpha)
{
	myData.myColor.w = aAlpha;
}

const CU::Vector2f& SpriteInstance::GetPosition() const
{
	return myData.myPosition;
}
const CU::Vector2f& SpriteInstance::GetSize() const
{
	return myData.mySize;
}
const CU::Vector2f& SpriteInstance::GetPivot() const
{
	return myData.myPivot;
}
const float SpriteInstance::GetRotation() const
{
	return myData.myRotation;
}
const CU::Vector4f& SpriteInstance::GetColor() const
{
	return myData.myColor;
}

const SpriteRenderData& SpriteInstance::GetData() const
{
	return myData;
}

Sprite* SpriteInstance::GetSprite() const
{
	return mySprite;
}
