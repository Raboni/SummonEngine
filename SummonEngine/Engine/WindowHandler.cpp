#include "stdafx.h"
#include "WindowHandler.h"
#include "EngineMessages.h"
#include "SpriteUnits.h"
#include <Input.hpp>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CU::Input::Get()->Update(uMsg, wParam, lParam);

	switch (uMsg)
	{
	case WM_SIZE:
		EnginePostMaster::GetInstance()->SendDelayedMessage(EngineMessages::ResolutionChanged);
		break;
	case WM_DESTROY:
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	default:
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

bool WindowHandler::Init(const WindowData& aWinData)
{
	WNDCLASS windowClass = {};
	windowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowClass.lpszClassName = L"GameEngine";
	windowClass.hIcon = (HICON)LoadImage(nullptr, L"icon.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_SHARED);
	RegisterClass(&windowClass);

	myWindowHandle = CreateWindowA("GameEngine", aWinData.myTitle.c_str(), WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_POPUP | WS_VISIBLE | WS_SIZEBOX | WS_MINIMIZEBOX,
		aWinData.myX, aWinData.myY, aWinData.myWidth, aWinData.myHeight,
		nullptr, nullptr, nullptr, nullptr);
	myData = aWinData;
	SetWindowSize(aWinData.myWidth, aWinData.myHeight);
	return true;
}

void WindowHandler::OpenConsole(const CU::Vector2i aPosition, const CU::Vector2i aSize, const std::string& aTitle)
{
	#ifndef _RETAIL
	AllocConsole();
	DWORD* processID = new DWORD();
	GetWindowThreadProcessId(myWindowHandle, processID);
	AttachConsole(*processID);

	unsigned int flags = SWP_NOZORDER;
	if (aSize == CU::Vector2i::Zero)
	{
		flags = flags | SWP_NOSIZE;
	}
	HWND consoleWindow = GetConsoleWindow();
	SetWindowPos(consoleWindow, 0, aPosition.x, aPosition.y, aSize.x, aSize.y, flags);
	SetConsoleTitleA(aTitle.c_str());
	HICON hIcon = (HICON)LoadImage(nullptr, L"icon.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_SHARED);
	SendMessage(consoleWindow, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	SendMessage(consoleWindow, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

	freopen_s(&myCinFile, "CONIN$", "r", stdin);
	freopen_s(&myCoutFile, "CONOUT$", "w", stdout);
	setvbuf(stdin, NULL, _IOFBF, BUFSIZ);
	setvbuf(stdout, NULL, _IOFBF, BUFSIZ);
	#endif
}
void WindowHandler::CloseConsole()
{
	#ifndef _RETAIL
	fclose(myCinFile);
	fclose(myCoutFile);
	FreeConsole();
	#endif
}

HWND WindowHandler::GetWindow()
{
	return myWindowHandle;
}

unsigned int WindowHandler::GetWidth()
{
	return myData.myWidth;
}
unsigned int WindowHandler::GetHeight()
{
	return myData.myHeight;
}

void WindowHandler::RecieveMessage(const CU::Message<EngineMessages>& aMessage)
{
	switch (aMessage.GetType())
	{
	case EngineMessages::ResolutionChanged:
		RECT rect;
		if (GetWindowRect(myWindowHandle, &rect))
		{
			SetWindowSize(rect.right - rect.left, rect.bottom - rect.top);
		}
		break;
	default:
		break;
	}
}

void WindowHandler::SetWindowSize(unsigned int aWidth, unsigned int aHeight)
{
	RECT windowRect;
	RECT clientRect;
	GetWindowRect(myWindowHandle, &windowRect);
	GetClientRect(myWindowHandle, &clientRect);
	const int borderX = (windowRect.right - windowRect.left) - (clientRect.right - clientRect.left);
	const int borderY = (windowRect.bottom - windowRect.top) - (clientRect.bottom - clientRect.top);

	myData.myWidth = aWidth;
	myData.myHeight = aHeight;
	myData.myWidth -= borderX;
	myData.myHeight -= borderY;
	CU::Input::Get()->SetWindowSize(CU::Vector2i(myData.myWidth, myData.myHeight));
	SpriteUnits::myBorderTop = -1.0f;
	SpriteUnits::myBorderBottom = 1.0f;
	SpriteUnits::myBorderRight = static_cast<float>(myData.myWidth) / myData.myHeight;
	SpriteUnits::myBorderLeft = -SpriteUnits::myBorderRight;
	SpriteUnits::myBorderWidth = SpriteUnits::myBorderRight * 2;
	SpriteUnits::myBorderHeight = 2.0f;
	SpriteUnits::myPixelSize = SpriteUnits::myBorderWidth / myData.myWidth;
}
