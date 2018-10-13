#pragma once

#include "Window.h"
#include <d3d11.h>
#include <dxgi.h>
#include <DirectXMath.h>

class DX11Renderer
{
public:
	DX11Renderer(Window& wnd);
	~DX11Renderer() = default;
	void newFrame();
	void endFrame();

	void setRenderTargets() { DeviceContex->OMSetRenderTargets(1, &renderTargetView, depthStencilView); };

	void setObjWVP(DirectX::XMMATRIX & x);

	ID3D11Device* getDevice();
	ID3D11DeviceContext* getDeviceContex();
	ID3D11DepthStencilView* getDepthStencilView();

	void UpdateCamPosition(float x, float y, float z);

	DirectX::XMMATRIX getWVP() { return WVP; };
private:

	struct cbPerObject
	{
		DirectX::XMMATRIX  WVP;
	};
	cbPerObject cbPerObj;

	void CreateDevice(Window& wnd);
	void CreateRenderTarget();

	//dx11
	IDXGISwapChain * SwapChain = nullptr;
	ID3D11Device* Device = nullptr; // creates - texture, shader vertex buffer etc
	ID3D11DeviceContext* DeviceContex = nullptr;
	ID3D11RenderTargetView* renderTargetView = nullptr; 

	//depth/3d stuff
	ID3D11DepthStencilView* depthStencilView;
	ID3D11Texture2D* depthStencilBuffer;

	D3D11_VIEWPORT viewport;

	ID3D11Buffer* cbPerObjectBuffer;

	DirectX::XMMATRIX WVP;

	DirectX::XMMATRIX World;
	DirectX::XMMATRIX camView;
	DirectX::XMMATRIX camProjection;

	DirectX::XMVECTOR camPosition;
	DirectX::XMVECTOR camTarget;
	DirectX::XMVECTOR camUp;

};
