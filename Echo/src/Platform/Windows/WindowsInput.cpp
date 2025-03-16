#include "pch.h"
#include "Echo/Core/Input.h"
#include "Echo/Core/Application.h"
#include <windows.h>

namespace Echo
{
	int GLFWKeyToVirtualKey(int glfw_key)
	{
		// Special keys
		if (glfw_key == 259) return 8;     // GLFW_KEY_BACKSPACE -> VK_BACK
		if (glfw_key == 258) return 9;     // GLFW_KEY_TAB -> VK_TAB
		if (glfw_key == 257) return 13;    // GLFW_KEY_ENTER -> VK_RETURN
		if (glfw_key == 284) return 19;    // GLFW_KEY_PAUSE -> VK_PAUSE
		if (glfw_key == 280) return 20;    // GLFW_KEY_CAPS_LOCK -> VK_CAPITAL
		if (glfw_key == 256) return 27;    // GLFW_KEY_ESCAPE -> VK_ESCAPE
		if (glfw_key == 32)  return 32;    // GLFW_KEY_SPACE -> VK_SPACE
		if (glfw_key == 266) return 33;    // GLFW_KEY_PAGE_UP -> VK_PRIOR
		if (glfw_key == 267) return 34;    // GLFW_KEY_PAGE_DOWN -> VK_NEXT
		if (glfw_key == 269) return 35;    // GLFW_KEY_END -> VK_END
		if (glfw_key == 268) return 36;    // GLFW_KEY_HOME -> VK_HOME
		if (glfw_key == 263) return 37;    // GLFW_KEY_LEFT -> VK_LEFT
		if (glfw_key == 265) return 38;    // GLFW_KEY_UP -> VK_UP
		if (glfw_key == 262) return 39;    // GLFW_KEY_RIGHT -> VK_RIGHT
		if (glfw_key == 264) return 40;    // GLFW_KEY_DOWN -> VK_DOWN
		if (glfw_key == 260) return 45;    // GLFW_KEY_INSERT -> VK_INSERT
		if (glfw_key == 261) return 46;    // GLFW_KEY_DELETE -> VK_DELETE

		// Numbers (0-9)
		if (glfw_key >= 48 && glfw_key <= 57)
		{
			return glfw_key;               // Same codes for 0-9
		}

		// Letters (A-Z)
		if (glfw_key >= 65 && glfw_key <= 90)
		{
			return glfw_key;               // Same codes for A-Z
		}

		// Numpad numbers
		if (glfw_key >= 320 && glfw_key <= 329)
		{
			return glfw_key + 224;         // Convert to VK_NUMPAD0 through VK_NUMPAD9
		}

		// Numpad operators
		if (glfw_key == 332) return 106;   // GLFW_KEY_KP_MULTIPLY -> VK_MULTIPLY
		if (glfw_key == 334) return 107;   // GLFW_KEY_KP_ADD -> VK_ADD
		if (glfw_key == 333) return 109;   // GLFW_KEY_KP_SUBTRACT -> VK_SUBTRACT
		if (glfw_key == 330) return 110;   // GLFW_KEY_KP_DECIMAL -> VK_DECIMAL
		if (glfw_key == 331) return 111;   // GLFW_KEY_KP_DIVIDE -> VK_DIVIDE

		// Function keys (F1-F24)
		if (glfw_key >= 290 && glfw_key <= 313)
		{
			return glfw_key - 178;         // Convert to VK_F1 through VK_F24
		}

		// Lock keys
		if (glfw_key == 282) return 144;   // GLFW_KEY_NUM_LOCK -> VK_NUMLOCK
		if (glfw_key == 281) return 145;   // GLFW_KEY_SCROLL_LOCK -> VK_SCROLL

		// Modifier keys
		if (glfw_key == 340) return 160;   // GLFW_KEY_LEFT_SHIFT -> VK_LSHIFT
		if (glfw_key == 344) return 161;   // GLFW_KEY_RIGHT_SHIFT -> VK_RSHIFT
		if (glfw_key == 341) return 162;   // GLFW_KEY_LEFT_CONTROL -> VK_LCONTROL
		if (glfw_key == 345) return 163;   // GLFW_KEY_RIGHT_CONTROL -> VK_RCONTROL

		if (glfw_key == 342) return 164;     // GLFW_KEY_LEFT_ALT -> VK_LMENU

		return 0;  // Return 0 for unmapped keys (different from the -2 in original)
	}


	bool Input::IsKeyPressed(int keyCode)
	{
		int vkCode = GLFWKeyToVirtualKey(keyCode);

		return (GetAsyncKeyState(vkCode) & 0x8000) != 0;
	}

	bool Input::IsMouseButtonPressed(int button)
	{
		// Convert GLFW button IDs to Windows mouse button constants
		int windowsButton;
		switch (button)
		{
			case 0: // GLFW_MOUSE_BUTTON_LEFT
				windowsButton = VK_LBUTTON;
				break;
			case 1: // GLFW_MOUSE_BUTTON_RIGHT
				windowsButton = VK_RBUTTON;
				break;
			case 2: // GLFW_MOUSE_BUTTON_MIDDLE
				windowsButton = VK_MBUTTON;
				break;
			case 3: // GLFW_MOUSE_BUTTON_4
				windowsButton = VK_XBUTTON1;
				break;
			case 4: // GLFW_MOUSE_BUTTON_5
				windowsButton = VK_XBUTTON2;
				break;
			default:
				return false;
		}

		return (GetAsyncKeyState(windowsButton) & 0x8000) != 0;
	}

	float Input::GetMouseX()
	{
		auto [x, y] = GetMousePosition();
		return x;
	}

	float Input::GetMouseY()
	{
		auto [x, y] = GetMousePosition();
		return y;
	}

	std::pair<float, float> Input::GetMousePosition()
	{
		HWND hwnd = static_cast<HWND>(Application::Get().GetWindow().GetNativeWindow());
		POINT p;

		GetCursorPos(&p);

		return { p.x, p.y };
	}
}