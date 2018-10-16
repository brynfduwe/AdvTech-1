#include "Input.h"

Input::Input(HINSTANCE hinst, HWND wnd, int width, int height)
{
	directInput = 0;
	keyboard = 0;
	mouse = 0;

	screenWidth = width;
	screenHeight = height;

	mouseX = 0;
	mouseY = 0;

	DirectInput8Create(hinst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, NULL);
	
	directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	keyboard->SetDataFormat(&c_dfDIKeyboard);
	keyboard->Acquire();

	directInput->CreateDevice(GUID_SysMouse, &mouse, NULL);
	mouse->SetDataFormat(&c_dfDIMouse);
	mouse->SetCooperativeLevel(wnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	mouse->Acquire();
}

Input::~Input()
{
}

void Input::Update(HWND wnd)
{
	HRESULT result;

	result = keyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);
	if (FAILED(result))
	{
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			keyboard->Acquire();
		}
	}

	result = mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouseState);
	if (FAILED(result))
	{
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			mouse->Acquire();
		}
	}

	//mouseX += mouseState.lX;
	//mouseY += mouseState.lY;

	POINT p;
	GetCursorPos(&p);
	ScreenToClient(wnd, &p);
	
	mouseX = p.x;
	mouseY = p.y;

	if(mouseX < 0) 
	{ 
		mouseX = 0; 
	}

	if(mouseY < 0)
	{
		mouseY = 0; 
	}

	if(mouseX > screenWidth)
	{ 
		mouseX = screenWidth; 
	}

	if(mouseY > screenHeight) 
	{ 
		mouseY = screenHeight; 
	}

}

int Input::getMouseX()
{
	return mouseX;
}

int Input::getMouseY()
{
	return mouseY;
}
