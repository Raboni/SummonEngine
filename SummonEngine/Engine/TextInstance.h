#pragma once
#include <Vector.hpp>
#include <string>

class Text;


class TextInstance
{
public:
	TextInstance();
	TextInstance(const TextInstance& aInstance);
	TextInstance& operator=(const TextInstance& aInstance);
	~TextInstance();

	void Init(const std::string& aText, const std::string& aFont);
	void Release();

	void SetColor(const CU::Vector3f& aColor);
	void SetColor(const CU::Vector4f& aColor);
	void SetPosition(const CU::Vector3f& aPosition);
	void SetPosition(const CU::Vector2f& aPosition);
	void SetScale(const CU::Vector2f& aScale);
	//Origin at top left
	void SetPivot(const CU::Vector2f& aPivot);

	const CU::Vector4f& GetColor() const;
	const CU::Vector3f& GetPosition() const;
	const CU::Vector2f& GetScale() const;
	const CU::Vector2f& GetPivot() const;

	const CU::Vector2f GetSizeInPixels() const;
	const std::string& GetText() const;
	const std::string& GetFont() const;

private:
	friend class TextRenderer;
	CU::Vector4f myColor;
	CU::Vector3f myPosition;
	CU::Vector2f myScale;
	CU::Vector2f myPivot;
	Text* myTextObject;
};