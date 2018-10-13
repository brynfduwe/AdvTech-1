#pragma once
#include<Windows.h>

class Window
{
public:
	Window(int width, int height);
	~Window() = default;

	HWND getHandle();
	void Test();

private:
	HWND m_handle;
};
