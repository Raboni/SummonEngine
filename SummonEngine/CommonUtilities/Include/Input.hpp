#pragma once

#ifdef DLL_EXPORTS
#define DLL_API
#else
#define DLL_API
#endif

#pragma comment(lib, "XInput.lib")

#include <Windows.h>
#include <Xinput.h>
#include <vector>
#include <bitset>
#include "Vector2.hpp"

namespace CU
{
	class Input
	{
	public:
		enum class KeyCode : UINT
		{
			None = 0,

			Num0 = '0',
			Num1, Num2, Num3, Num4,
			Num5, Num6, Num7, Num8, Num9,

			NumPad0 = VK_NUMPAD0,
			NumPad1, NumPad2, NumPad3, NumPad4,
			NumPad5, NumPad6, NumPad7, NumPad8, NumPad9,

			A = 'A',
			B, C, D, E, F, G, H, I, J, K, L, M, N,
			O, P, Q, R, S, T, U, V, W, X, Y, Z,

			/*a = 'a',
			b, c, d, e, f, g, h, i, j, k, l, m, n,
			o, p, q, r, s, t, u, v, w, x, y, z,*/

			F1 = VK_F1,
			F2, F3, F4, F5, F6, F7, F8, F9, F10,
			F11, F12, F13, F14, F15, F16, F17, F18, F19,
			F20, F21, F22, F23, F24,

			Back = VK_BACK,
			Tab,

			Return = VK_RETURN,

			Shift = VK_SHIFT,
			Control, Alt,

			CapsLock = VK_CAPITAL,

			Escape = VK_ESCAPE,

			Space = VK_SPACE,

			Left = VK_LEFT,
			Up, Right, Down,

			Insert = VK_INSERT,
			Delete
		};
		enum class MouseButton
		{
			None,
			Left,
			Right,
			Middle,
			X1,
			X2
		};
		enum class XboxButton : WORD
		{
			None = 0,
			A = XINPUT_GAMEPAD_A,
			B = XINPUT_GAMEPAD_B,
			X = XINPUT_GAMEPAD_X,
			Y = XINPUT_GAMEPAD_Y,
			RB = XINPUT_GAMEPAD_RIGHT_SHOULDER,
			LB = XINPUT_GAMEPAD_LEFT_SHOULDER,
			DPadUp = XINPUT_GAMEPAD_DPAD_UP,
			DPadDown = XINPUT_GAMEPAD_DPAD_DOWN,
			DPadLeft = XINPUT_GAMEPAD_DPAD_LEFT,
			DPadRight = XINPUT_GAMEPAD_DPAD_RIGHT,
			Start = XINPUT_GAMEPAD_START,
			Back = XINPUT_GAMEPAD_BACK,
			LThumb = XINPUT_GAMEPAD_LEFT_THUMB,
			RThumb = XINPUT_GAMEPAD_RIGHT_THUMB
		};
		/*enum class XboxButton : UINT
		{
			None = 0,
			A = VK_PAD_A,
			B,
			X,
			Y,
			RB = VK_PAD_RSHOULDER,
			LB,
			LTrigger = VK_PAD_LTRIGGER,
			RTrigger,
			DPadUp = VK_PAD_DPAD_UP,
			DPadDown,
			DPadLeft,
			DPadRight,
			Start = VK_PAD_START,
			Back,
			LThumb = VK_PAD_LTHUMB_PRESS,
			RThumb = VK_PAD_RTHUMB_PRESS
		};*/
		enum class XboxAxis : unsigned short
		{
			None = 0,
			DPad,
			LStick,
			RStick,
			LTrigger,
			RTrigger
		};

		static void Create();
		static void Destroy();
		static Input* Get();

		bool Update(UINT aMessage, WPARAM aWParam, LPARAM aLParam);
		void UpdateXbox();
		void UpdateConnected();
		void UpdateReset();

		void SetWindowSize(const Vector2i& aSize);
		void SetWindowSize(const Vector2f& aSize);

		bool GetXboxConnected(unsigned int aController = 0);
		bool GetXboxDown(XboxButton aButton, unsigned int aController = 0);
		bool GetXboxPressed(XboxButton aButton, unsigned int aController = 0);
		bool GetXboxReleased(XboxButton aButton, unsigned int aController = 0);
		Vector2f GetXboxAxis(XboxAxis aAxis, unsigned int aController = 0);
		void SetXboxDeadZone(unsigned int aDeadZone);

		bool GetKeyDown(KeyCode aKey);
		bool GetKeyPressed(KeyCode aKey);
		bool GetKeyReleased(KeyCode aKey);

		bool GetMouseButtonDown(MouseButton aButton);
		bool GetMouseButtonPressed(MouseButton aButton);
		bool GetMouseButtonReleased(MouseButton aButton);
		int GetMouseScroll();

		int GetCursorScreenX();
		int GetCursorScreenY();
		int GetCursorWindowX();
		int GetCursorWindowY();
		int GetCursorXDelta();
		int GetCursorYDelta();
		const Vector2f GetCursorDXSpace();
		const Vector2f GetCursorSpriteSpace();
		void SetCursor(int aX, int aY);

	private:
		Input() = default;
		~Input() = default;

		static Input* ourInstance;

		std::vector<UINT> myKeys;
		std::vector<UINT> myKeysNew;
		std::vector<UINT> myKeysRemoved;

		Vector2f myXboxAxis[XUSER_MAX_COUNT][static_cast<unsigned short>(XboxAxis::RTrigger) + 1];
		DWORD myXboxPrevPacket[XUSER_MAX_COUNT];
		WORD myXboxButtonState[XUSER_MAX_COUNT];
		WORD myXboxButtonStatePrev[XUSER_MAX_COUNT];

		Vector2i myWindowSize;
		float myWindowAspectRatio;

		unsigned int myXboxStickDeadZone = 6000;
		int myMouseScroll;
		int myCursorPosX;
		int myCursorPosY;
		int myCursorPosXPrev;
		int myCursorPosYPrev;
		int myCursorPosXWindow;
		int myCursorPosYWindow;

		std::bitset<XUSER_MAX_COUNT> myXboxConnected;
		std::bitset<5> myMouseButtons;
		std::bitset<5> myMouseButtonsPressed;
		std::bitset<5> myMouseButtonsReleased;
	};
}