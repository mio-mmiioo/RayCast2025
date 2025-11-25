#include "Input.h"
//#include <DirectXMath.h>

namespace Input
{
	LPDIRECTINPUT8   pDInput = nullptr;
	LPDIRECTINPUTDEVICE8 pKeyDevice = nullptr; // デバイスオブジェクト
	BYTE keyState[256] = { 0 }; // 現在の各キーの状態
	BYTE prevKeyState[256];     //前フレームでの各キーの状態

	LPDIRECTINPUTDEVICE8 pMouseDevice = nullptr;
	DIMOUSESTATE mouseState; // マウスの状態
	DIMOUSESTATE prevMouseState; // 前回のマウスの状態
	XMVECTOR mousePosition; // マウスカーソルの位置を入れておく変数

	void Initialize(HWND hWnd)
	{
		DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&pDInput, nullptr);
		pDInput->CreateDevice(GUID_SysKeyboard, &pKeyDevice, nullptr);
		pKeyDevice->SetDataFormat(&c_dfDIKeyboard);
		pKeyDevice->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);

		pDInput->CreateDevice(GUID_SysMouse, &pMouseDevice, nullptr);
		pMouseDevice->SetDataFormat(&c_dfDIMouse);
		pMouseDevice->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	}

	void Update()
	{
		//memcpy(prevKeyState, keyState, sizeof(keyState)); // コピー先(ディスト) コピー元(ソース), サイズ

		for (int i = 0; i < 256; i++)
		{
			prevKeyState[i] = keyState[i];
		}

		pKeyDevice->Acquire();
		pKeyDevice->GetDeviceState(sizeof(keyState), &keyState);

		// マウスの状態を保存
		pMouseDevice->Acquire();
		memcpy(&prevMouseState, &mouseState, sizeof(mouseState));
		pMouseDevice->GetDeviceState(sizeof(mouseState), &mouseState);
	}

	bool IsKey(int keyCode)
	{
		if (keyState[keyCode] & 0x80)
		{
			return true;
		}

		return false;
	}

	bool IsKeyDown(int keyCode)
	{
		//今は押してて、前回は押してない
		//if ((keyState[keyCode] ^ prevKeyState[keyCode]) && keyState[keyCode])
		if (IsKey(keyCode) && !(prevKeyState[keyCode] & 0x80))
		{
			return true;
		}
		return false;
	}

	bool IsKeyUp(int keyCode)
	{
		//今は離してて、前回は押してる
		//if ((keyState[keyCode] ^ prevKeyState[keyCode]) && prevKeyState[keyCode])
		if (!IsKey(keyCode) && (prevKeyState[keyCode] & 0x80))
		{
			return true;
		}
		return false;
	}

	XMVECTOR GetMousePosition()
	{
		return mousePosition;
	}

	void SetMousePosition(int x, int y)
	{
		mousePosition = XMVectorSet((float)x, (float)y, 0, 0);
	}

	bool IsMouseButton(int buttonCode)
	{
		if (mouseState.rgbButtons[buttonCode] & 0x80)
		{
			return true;
		}
		return false;
	}

	bool IsButtonUp(int buttonCode)
	{
		if ((mouseState.rgbButtons[buttonCode] ^ prevMouseState.rgbButtons[buttonCode]) && prevMouseState.rgbButtons[buttonCode])
		//if (!IsKeey(KeyCode) && (prevKyState[keyCode] & 0x80)
		{
			return true;
		}
		return false;
	}

	bool IsButtonDown(int buttonCode)
	{
		if ((mouseState.rgbButtons[buttonCode] ^ prevMouseState.rgbButtons[buttonCode]) && mouseState.rgbButtons[buttonCode])
		//if (IsKey(keyCode) && !(prevKeyState[keyCode] & 0x80))
		{
			return true;
		}
		return false;
	}

	void Release()
	{
		SAFE_RELEASE(pDInput);
	}
}