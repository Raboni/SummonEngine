#pragma once
#include "EngineMessages.h"
#include <Windows.h>
#include <Vector2.hpp>
#include <string>

class WindowHandler : public CU::Subscriber<EngineMessages>
{
public:
	struct WindowData
	{
		std::string myTitle = "Game Engine";
		int myX = 100;
		int myY = 100;
		unsigned int myWidth = 1280;
		unsigned int myHeight = 720;
	};

	WindowHandler() = default;
	~WindowHandler() = default;

	bool Init(const WindowData& aWinData);
	void OpenConsole(const CU::Vector2i aPosition = CU::Vector2i::Zero, const CU::Vector2i aSize = CU::Vector2i::Zero, const std::string& aTitle = "Debug Console");
	void CloseConsole();

	HWND GetWindow();
	unsigned int GetWidth();
	unsigned int GetHeight();
	template<typename T>
	CU::Vector2<T> GetWindowSize();

	void RecieveMessage(const CU::Message<EngineMessages>& aMessage) override;
	void SetWindowSize(unsigned int aWidth, unsigned int aHeight);

private:
	WindowData myData;
	HWND myWindowHandle;
	FILE* myCinFile;
	FILE* myCoutFile;
};

template<typename T>
inline CU::Vector2<T> WindowHandler::GetWindowSize()
{
	return CU::Vector2<T>(static_cast<T>(myData.myWidth), static_cast<T>(myData.myHeight));
}
