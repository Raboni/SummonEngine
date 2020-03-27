#pragma once

class SpriteUnits
{
public:
	static const float GetBorderTop();
	static const float GetBorderBottom();
	static const float GetBorderLeft();
	static const float GetBorderRight();
	static const float GetBorderWidth();
	static const float GetBorderHeight();

	static const float GetPixelSize();

private:
	friend class WindowHandler;

	static float myBorderTop;
	static float myBorderBottom;
	static float myBorderLeft;
	static float myBorderRight;
	static float myBorderWidth;
	static float myBorderHeight;

	static float myPixelSize;
};