#include <Windows.h>
#include "Window.h"
#include "DX11Renderer.h"
#include "Triangle.h"
#include "Input.h"
#include <vector>
#include <DirectXMath.h>

int WinMain(HINSTANCE appInstance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdCount)
{
	Window window(800, 500);
	Input input(appInstance, window.getHandle(), 800, 500);
	DX11Renderer renderer(window);

	//std::vector<Triangle> floor;
	//float x = -10;
	//float z = -10;
	//for (int rz = 0; rz < 10; rz++)
	//{
	//	x = 0;
	//	for (int cx = 0; cx < 10; cx++)
	//	{
	//		Triangle t(renderer, x, -2.0f, z, 0.0f);
	//		floor.push_back(t);
	//		x += 2;
	//	}
	//	z += 2;
	//}

	Triangle triangle(renderer, -6.0f, 6.0f, -6.0f, -0.05f);
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
		

			//	float x = (((2 * input.getMouseX()) / 800) - 1);
			//	float y = (((2 * input.getMouseY()) / 500) - 1) * -1;

				DirectX::XMMATRIX projM;

				float x = (((2.0f * input.getMouseX()) / 800) - 1);
				float y = -(((2.0f * input.getMouseY()) / 500) - 1);


				triangle2.SetNewPos(x, y, 0);
			}
		}

		//main loop - update and render
		input.Update();

		renderer.newFrame();

		//for (int i = 0; i < 100; i++)
		//{
		//	floor[i].Update();
		//	floor[i].Draw(renderer);
		//}

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