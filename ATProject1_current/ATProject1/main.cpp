#include <Windows.h>
#include "Window.h"
#include "DX11Renderer.h"
#include "Triangle.h"
#include "Input.h"
#include "GridSpace.h"
#include <vector>
#include <DirectXMath.h>

void pickRay(DirectX::XMVECTOR& pickRayInWorldSpacePos, DirectX::XMVECTOR& pickRayInWorldSpaceDir, DX11Renderer r, Input i)
{
	DirectX::XMVECTOR pickRayInViewSpaceDir = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR pickRayInViewSpacePos = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	DirectX::XMFLOAT4X4 projM; // THIS NEEDS TO BE CAMPROJ ME THINKS
							   //convert matrix to float4x4 for picking
	DirectX::XMStoreFloat4x4(&projM, r.getProjMatrix());

	float x = (((2.0f * (float)i.getMouseX()) / (float)800) - 1) / projM(0, 0);
	float y = -(((2.0f * (float)i.getMouseY()) / (float)500) - 1) / projM(1, 1);

	pickRayInViewSpaceDir = DirectX::XMVectorSet(x, y, 1, 0);

	DirectX::XMMATRIX pickToWorldPosMat;
	DirectX::XMVECTOR nullVec;

	//inverse view space matrix
	pickToWorldPosMat = DirectX::XMMatrixInverse(&nullVec, r.getViewMatrix());

	pickRayInWorldSpacePos = DirectX::XMVector3TransformCoord(pickRayInViewSpacePos, pickToWorldPosMat);
	pickRayInWorldSpaceDir = DirectX::XMVector3TransformNormal(pickRayInViewSpaceDir, pickToWorldPosMat);
}

bool objRayCollisionCheck(std::vector<unsigned short> indicesArray, std::vector<DirectX::XMFLOAT3> verticiesArray, DirectX::XMMATRIX world, DirectX::XMVECTOR prwsPos, DirectX::XMVECTOR prwsDir)
{
	float tDist = 0;
	float cDist = 1000;
	int hit = -1;

	for (int i = 0; i < indicesArray.size() / 3; i++)
	{
		//Triangle's vertices V1, V2, V3
		DirectX::XMVECTOR tri1V1 = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		DirectX::XMVECTOR tri1V2 = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		DirectX::XMVECTOR tri1V3 = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		//Temporary 3d floats for each vertex
		DirectX::XMFLOAT3 tV1, tV2, tV3;

		//Get triangle 
		tV1 = verticiesArray[indicesArray[(i * 3) + 0]];
		tV2 = verticiesArray[indicesArray[(i * 3) + 1]];
		tV3 = verticiesArray[indicesArray[(i * 3) + 2]];

		tri1V1 = DirectX::XMVectorSet(tV1.x, tV1.y, tV1.z, 0.0f);
		tri1V2 = DirectX::XMVectorSet(tV2.x, tV2.y, tV2.z, 0.0f);
		tri1V3 = DirectX::XMVectorSet(tV3.x, tV3.y, tV3.z, 0.0f);

		//Transform the vertices to world space
		tri1V1 = DirectX::XMVector3TransformCoord(tri1V1, world);
		tri1V2 = DirectX::XMVector3TransformCoord(tri1V2, world);
		tri1V3 = DirectX::XMVector3TransformCoord(tri1V3, world);

		//Find the normal using U, V coordinates (two edges)
		DirectX::XMVECTOR U = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		DirectX::XMVECTOR V = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		DirectX::XMVECTOR faceNormal = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		U = DirectX::XMVectorSubtract(tri1V2, tri1V1);
		V = DirectX::XMVectorSubtract(tri1V3, tri1V1);

		//Compute face normal by crossing U, V
		faceNormal = DirectX::XMVector3Cross(U, V);

		faceNormal = DirectX::XMVector3Normalize(faceNormal);

		//Calculate a point on the triangle for the plane equation
		DirectX::XMVECTOR triPoint = tri1V1;

		//Get plane equation ("Ax + By + Cz + D = 0") Variables
		float tri1A = DirectX::XMVectorGetX(faceNormal);
		float tri1B = DirectX::XMVectorGetY(faceNormal);
		float tri1C = DirectX::XMVectorGetZ(faceNormal);
		float tri1D = (-tri1A * DirectX::XMVectorGetX(triPoint) - tri1B * DirectX::XMVectorGetY(triPoint) - tri1C * DirectX::XMVectorGetZ(triPoint));

		//Now we find where (on the ray) the ray intersects with the triangles plane
		float ep1, ep2, t = 0.0f;
		float planeIntersectX, planeIntersectY, planeIntersectZ = 0.0f;
		DirectX::XMVECTOR pointInPlane = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		ep1 = (DirectX::XMVectorGetX(prwsPos) * tri1A) + (DirectX::XMVectorGetY(prwsPos) * tri1B) + (DirectX::XMVectorGetZ(prwsPos) * tri1C);
		ep2 = (DirectX::XMVectorGetX(prwsDir) * tri1A) + (DirectX::XMVectorGetY(prwsDir) * tri1B) + (DirectX::XMVectorGetZ(prwsDir) * tri1C);

		//Make sure there are no divide-by-zeros
		if (ep2 != 0.0f)
			t = -(ep1 + tri1D) / (ep2);

		if (t > 0.0f)
		{
			//Get the point on the plane
			planeIntersectX = DirectX::XMVectorGetX(prwsPos) + DirectX::XMVectorGetX(prwsDir) * t;
			planeIntersectY = DirectX::XMVectorGetY(prwsPos) + DirectX::XMVectorGetY(prwsDir) * t;
			planeIntersectZ = DirectX::XMVectorGetZ(prwsPos) + DirectX::XMVectorGetZ(prwsDir) * t;

			pointInPlane = DirectX::XMVectorSet(planeIntersectX, planeIntersectY, planeIntersectZ, 0.0f);

			tDist = 1000;

			DirectX::XMVECTOR cp1 = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(tri1V3, tri1V2), DirectX::XMVectorSubtract(pointInPlane, tri1V2));
			DirectX::XMVECTOR cp2 = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(tri1V3, tri1V2), DirectX::XMVectorSubtract(tri1V1, tri1V2));
			if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(cp1, cp2)) >= 0)
			{
				cp1 = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(tri1V3, tri1V1), DirectX::XMVectorSubtract(pointInPlane, tri1V1));
				cp2 = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(tri1V3, tri1V1), DirectX::XMVectorSubtract(tri1V2, tri1V1));
				if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(cp1, cp2)) >= 0)
				{
					cp1 = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(tri1V2, tri1V1), DirectX::XMVectorSubtract(pointInPlane, tri1V1));
					cp2 = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(tri1V2, tri1V1), DirectX::XMVectorSubtract(tri1V3, tri1V1));
					if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(cp1, cp2)) >= 0)
					{
						tDist = t / 2;
						if (tDist < cDist)
						{
							cDist = tDist;
							hit = i;
							//triangle.hideObj();
							return true;
						}
					}
				}
			}
		}
	}

	return false;
}


int WinMain(HINSTANCE appInstance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdCount)
{
	Window window(800, 500);
	Input input(appInstance, window.getHandle(), 800, 500);
	DX11Renderer renderer(window);
	std::vector<Triangle> allObjects;

	std::vector<GridSpace> floor;
	float x = -10;
	float z = -10;
	for (int rz = 0; rz < 15; rz++)
	{
		x = 0;
		for (int cx = 0; cx < 15; cx++)
		{
			if ((cx == 5 && rz == 7) || (cx == 6 && rz == 7) || (cx == 7 && rz == 7) || (cx == 8 && rz == 7) || (cx == 9 && rz == 7) 
				|| (cx == 5 && rz == 8) || (cx == 6 && rz == 8) || (cx == 7 && rz == 8) || (cx == 8 && rz == 8) || (cx == 9 && rz == 8)
					|| (cx == 5 && rz == 9) || (cx == 6 && rz == 9) || (cx == 7 && rz == 9) || (cx == 8 && rz == 9) || (cx == 9 && rz == 9)
						|| (cx == 5 && rz == 3) || (cx == 6 && rz == 3) || (cx == 7 && rz == 3) || (cx == 5 && rz == 4) || (cx == 6 && rz == 4) || (cx == 7 && rz == 4))
			{
				GridSpace gs(renderer, x, -2.0f, z, 0.0f, true);
				floor.push_back(gs);
			}
			else
			{
				GridSpace gs(renderer, x, -2.0f, z, 0.0f, false);
				floor.push_back(gs);
			}
			x += 2;
		}
		z += 2;
	}

	int selected;
	int moveToSpace;

	for (int i = 0; i < 10; i++)
	{
		Triangle triangle(renderer, 0.0f, 0.0f, i * 3, 0.0f);
		allObjects.push_back(triangle);
	}


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
				input.Update(window.getHandle());

				DirectX::XMVECTOR prwsPos, prwsDir;
				pickRay(prwsPos, prwsDir, renderer, input);

				bool pickedThisFrame = false;

				for (int i = 0; i < allObjects.size(); i++)
				{
					if (objRayCollisionCheck(allObjects[i].getIndicies(), allObjects[i].getVericies(), allObjects[i].getObjWorld(), prwsPos, prwsDir))
					{
						//allObjects[i].hideObj();
						selected = i;
						pickedThisFrame = true;
					}
				}

				if (selected > -1 && !pickedThisFrame)
				{
					for (int i = 0; i < floor.size(); i++)
					{
						if (objRayCollisionCheck(floor[i].getIndicies(), floor[i].getVericies(), floor[i].getObjWorld(), prwsPos, prwsDir))
						{
							if (selected > -1)
							{
								//floor[i].hideObj();
								moveToSpace = i;

								GridSpace startSpace = floor[0];
								GridSpace targetSpace = floor[0];

								DirectX::XMFLOAT3 origin = DirectX::XMFLOAT3(allObjects[selected].getX(), allObjects[selected].getY(), allObjects[selected].getZ());
								DirectX::XMFLOAT3 target = DirectX::XMFLOAT3(floor[moveToSpace].getX(), floor[moveToSpace].getY(), floor[moveToSpace].getZ());

								//update move costs
								for (int i = 0; i < floor.size(); i++)
								{
									floor[i].SetDists(origin, target);
								}

								for (int i = 0; i < floor.size(); i++)
								{
									if(floor[i].getOriginDist() < startSpace.getOriginDist())
									startSpace = floor[i];

									if (floor[i].getEndDist() < targetSpace.getEndDist())
										targetSpace = floor[i];
								}

								bool endFound = false;
								int loops = 0;
								GridSpace nearSpace = startSpace;

								std::vector<DirectX::XMFLOAT3> pathList;
								std::vector<GridSpace> neighbors;
								
								pathList.push_back(DirectX::XMFLOAT3(startSpace.getX(), startSpace.getY(), startSpace.getZ()));

								while (!endFound)
								{
									neighbors.clear();
									//get neighbors
									for (int i = 0; i < floor.size(); i++)
									{
										int distX = (floor[i].getX() - nearSpace.getX()) * (floor[i].getX() - nearSpace.getX());
										int distZ = (floor[i].getZ() - nearSpace.getZ()) * (floor[i].getZ() - nearSpace.getZ());
										float dist = (sqrt(distX + distZ));

										if (dist < 2.5f && floor[i].getIsObstacle() == false)
											neighbors.push_back(floor[i]);

									}

									//A*
									for (int i = 0; i < neighbors.size(); i++)
									{
										if (neighbors[i].getEndDist() < nearSpace.getEndDist())
										{
											nearSpace = neighbors[i];
											pathList.push_back(DirectX::XMFLOAT3(nearSpace.getX(), nearSpace.getY(), nearSpace.getZ()));
										}

										if ((neighbors[i].getX() == floor[moveToSpace].getX() && neighbors[i].getZ() == floor[moveToSpace].getZ()) || loops > 1000)
										{
											endFound = true;
											break;
										}
									}

									if (endFound)
									{
										break;
									}

									loops++;
								}

								//add final space to list;
								pathList.push_back(target);

								//send list to unit
								if (loops < 1000)
								allObjects[selected].SetPathList(pathList);

								// allObjects[selected].SetNewPos(pathList[pathList.size() - 1].x, 0, pathList[pathList.size() - 1].z, false);
							//	allObjects[selected].SetNewPos(floor[moveToSpace].getX(), 0, floor[moveToSpace].getZ(), false);
							}
							selected = -1;
							moveToSpace = -1;
						}
					}
				}
			}
		}
	
		// exit input	
		//main loop - update and render

		renderer.newFrame();

		for (int i = 0; i < floor.size(); i++)
		{
			floor[i].Update();
			floor[i].Draw(renderer);
		}

		for (int i = 0; i < allObjects.size(); i++)
		{
			allObjects[i].Update();
			allObjects[i].Draw(renderer);
		}

		renderer.endFrame(); 
	}

	return 0;
}

