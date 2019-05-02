#pragma once

#include <Windows.h>
#include "Window.h"

LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
{

	if (msg == WM_DESTROY || msg == WM_CLOSE)
	{
		PostQuitMessage(0);
		return 0;
	}

	if (msg == WM_KEYDOWN)
	{
		if (GetKeyState('A') & 0x8000)
		{
		//	MessageBox(hWnd, "A", "Key Pressed", MB_OK);
		}

		if (GetKeyState('S') & 0x8000)
		{
		//	MessageBox(hWnd, "S", "Key Pressed", MB_OK);
		}

		if (GetKeyState('D') & 0x8000)
		{
			//MessageBox(hWnd, "D", "Key Pressed", MB_OK);
		}
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

Window::Window(int width, int height)
{
	//window style
	WNDCLASS wndc = { 0 };
	wndc.style = CS_OWNDC;
	wndc.lpfnWndProc = WinProc; //inputs;
	wndc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndc.lpszClassName = "AT1DX11";
	RegisterClass(&wndc);

	m_handle = CreateWindow("AT1DX11",
		"AT1 - DX11 - 15007649",
		WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
		100, 
		100, 
		width, 
		height,
		nullptr, nullptr, nullptr, nullptr);
}

HWND Window::getHandle()
{
	return m_handle;
}

