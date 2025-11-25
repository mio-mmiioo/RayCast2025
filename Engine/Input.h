#pragma once
#include <dInput.h>
#include <DirectXMath.h>

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dInput8.lib")

#define SAFE_RELEASE(p) if(p != nullptr){ p->Release(); p = nullptr;}

using namespace DirectX;

namespace Input
{
	void Initialize(HWND hWnd);
	void Update();
	bool IsKey(int keyCode);
	bool IsKeyDown(int keyCode);
	bool IsKeyUp(int keyCode);

	XMVECTOR GetMousePosition();
	void SetMousePosition(int x, int y);
	bool IsMouseButton(int buttonCode);
	bool IsButtonUp(int buttonCode);
	bool IsButtonDown(int buttonCode);
	void Release();
};