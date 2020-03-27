#pragma once
#include "Sprite.h"
#include <string>
#include <Vector2.hpp>
#include <Vector4.hpp>

struct SpriteRenderData
{
	CU::Vector4f myColor;
	CU::Vector2f myPosition;
	CU::Vector2f mySize;
	CU::Vector2f myPivot;
	float myRotation;
};

class SpriteInstance
{
public:
	SpriteInstance();
	SpriteInstance(const std::string& aPath, const CU::Vector2f& aPosition = CU::Vector2f::Zero, const CU::Vector2f& aSize = CU::Vector2f(0.1f, 0.1f));
	SpriteInstance(const SpriteInstance& aInstance);
	SpriteInstance& operator=(const SpriteInstance& aInstance);
	~SpriteInstance();
	void Init(const std::string& aPath, const CU::Vector2f& aPosition = CU::Vector2f::Zero, const CU::Vector2f& aSize = CU::Vector2f(0.1f, 0.1f));
	void Release();

	void SetSprite(const std::string& aPath);
	void SetPosition(const CU::Vector2f& aPosition);
	void SetSize(const CU::Vector2f& aSize);
	void SetPivot(const CU::Vector2f& aPivot);
	void SetRotation(const float aRotation);
	void SetColor(const CU::Vector4f& aColor);
	void SetColor(const CU::Vector3f& aColor);
	void SetAlpha(const float aAlpha);

	const CU::Vector2f& GetPosition() const;
	const CU::Vector2f& GetSize() const;
	const CU::Vector2f& GetPivot() const;
	const float GetRotation() const;
	const CU::Vector4f& GetColor() const;

	const SpriteRenderData& GetData() const;
	Sprite* GetSprite() const;

private:
	SpriteRenderData myData;
	std::string myPath;
	Sprite* mySprite;
};