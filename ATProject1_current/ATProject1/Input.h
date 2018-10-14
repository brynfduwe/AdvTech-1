#pragma once
#include <d3d11.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <dinput.h>

#pragma comment(lib, "dxguid.lib")

class Input
{
public:
	Input(HINSTANCE, HWND, int, int);
	~Input();

	void Update();

private:

	IDirectInput8 * directInput;
	IDirectInputDevice8* keyboard;
	IDirectInputDevice8* mouse;

	unsigned char keyboardState[256];
	DIMOUSESTATE mouseState;

	int screenWidth, screenHeight;
	int mouseX, mouseY;

};
