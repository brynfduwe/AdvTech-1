#include <Windows.h>
#include "Window.h"
#include "DX11Renderer.h"
#include "Triangle.h"
#include "Input.h"

int WinMain(HINSTANCE appInstance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdCount)
{
	Window window(800, 500);
	Input input(appInstance, window.getHandle(), 800, 500);
	DX11Renderer renderer(window);
	Triangle triangle(renderer, -3.0f, -3.0f, -3.0f, -0.05f);
	Triangle triangle2(renderer, 0.0f, 0.0f, 0.0f, 0.0f);
	Triangle triangle3(renderer, 3.0f, 3.0f, 3.0f, 0.05f);

	MSG msg = { 0 };
	while (true)
	{
		//check and remove input message
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT) break;

			if (msg.message == WM_CHAR)
			{
				if (msg.wParam == 'q')
				{
					renderer.UpdateCamPosition(0, 0, -0.5f);
				}
				
				if (msg.wParam == 'e')
				{
					renderer.UpdateCamPosition(0, 0, 0.5f);
				}
				if (msg.wParam == 'a')
				{
					renderer.UpdateCamPosition(-0.5f, 0, 0);
				}

				if (msg.wParam == 'd')
				{
					renderer.UpdateCamPosition(0.5f, 0, 0);
				}

			}

			if (msg.message == WM_KEYDOWN)
			{

				if (msg.wParam == VK_UP)
				{

				}
				if (GetKeyState('S') & 0x8000)
				{
					renderer.UpdateCamPosition(0, -0.5f, 0);
				}

				if (GetKeyState('W') & 0x8000)
				{
					renderer.UpdateCamPosition(0, 0.5f, 0);
				}

			}

			if (msg.message == WM_LBUTTONDOWN)
			{
				input.Update();
			}
		}

		//main loop - update and render
		input.Update();

		renderer.newFrame();

		triangle.Update();
		triangle2.Update();
		triangle3.Update();

		triangle.Draw(renderer);
		triangle2.Draw(renderer);
		triangle3.Draw(renderer);

		renderer.endFrame(); 
	}

	return 0;
}