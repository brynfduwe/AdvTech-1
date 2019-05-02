#include "DX11Renderer.h"

DX11Renderer::DX11Renderer(Window& wnd)
{
	CreateDevice(wnd);
	CreateRenderTarget();
}

struct cbPerObject
{
	DirectX::XMMATRIX  WVP;
};

cbPerObject cbPerObj;

void DX11Renderer::releaseBuffers()
{
	SwapChain->Release();
	Device->Release();
	DeviceContex->Release();
	renderTargetView->Release();
	depthStencilView->Release();
	depthStencilBuffer->Release();
	cbPerObjectBuffer->Release();
}

void DX11Renderer::newFrame()
{
	//output merger stage
	DeviceContex->OMSetRenderTargets(1, &renderTargetView, nullptr); // last one is used for 3d!

	auto viewport = CD3D11_VIEWPORT(.0f, .0f, 800.f, 500.f);
	DeviceContex->RSSetViewports(1, &viewport);

//	float r = rand() % 255;
//	float g = rand() % 255;
//	float b = rand() % 255;

	//Clear render target view
	float clearColor[] = { 0, 0.3f, 0.7f, 1 }; // RBG
	DeviceContex->ClearRenderTargetView(renderTargetView, clearColor);

	World = DirectX::XMMatrixIdentity();
	WVP = World * camView * camProjection;
	cbPerObj.WVP = XMMatrixTranspose(WVP);

	DeviceContex->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
	//DeviceContex->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
}

void DX11Renderer::endFrame()
{
	//buffer swap;
	SwapChain->Present(1, 0);
}

void DX11Renderer::setObjWVP(DirectX::XMMATRIX & x)
{
	WVP = x * camView * camProjection;
	cbPerObj.WVP = XMMatrixTranspose(WVP);
	DeviceContex->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
	DeviceContex->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
}

ID3D11Device * DX11Renderer::getDevice()
{
	return Device;
}

ID3D11DeviceContext * DX11Renderer::getDeviceContex()
{
	return DeviceContex;
}

ID3D11DepthStencilView * DX11Renderer::getDepthStencilView()
{
	return depthStencilView;
}

void DX11Renderer::UpdateCamPosition(float x, float y, float z)
{
	DirectX::XMVECTOR a = DirectX::XMVectorSet(x, y, z, 0);
	camPosition = DirectX::XMVectorAdd(a, camPosition);

	//View matrix
	camView = DirectX::XMMatrixLookAtLH(camPosition, camTarget, camUp);

	//Projection matrix
	camProjection = DirectX::XMMatrixPerspectiveFovLH(0.4f*3.14f, (float)800 / 500, 0.01f, 1000.0f);

}

void DX11Renderer::CreateDevice(Window& wnd)
{
	//initialise directx
	//create swap chain
	DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
	swapChainDesc.BufferCount = 1; // 1 = double buffer, 2 is triple etc
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // draw on display
	swapChainDesc.OutputWindow = wnd.getHandle();
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Windowed = true; // fullscreen?

								   
	D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION,
		&swapChainDesc, &SwapChain, &Device, nullptr, &DeviceContex);
}

void DX11Renderer::CreateRenderTarget()
{
	//backbuffer
	ID3D11Texture2D* backBuffer;
	SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	Device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
	backBuffer->Release();

	//depth
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = 800;
	depthStencilDesc.Height = 500;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	//Depth/Stencil View
	Device->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
	Device->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);

	//Render Target
	DeviceContex->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

	viewport.Width = (float)800;
	viewport.Height = (float)500;
	viewport.MinDepth = 0.01f;
	viewport.MaxDepth = 1000.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	DeviceContex->RSSetViewports(1, &viewport);

	//constant buffer
	D3D11_BUFFER_DESC consantBuffer;
	ZeroMemory(&consantBuffer, sizeof(D3D11_BUFFER_DESC));

	consantBuffer.Usage = D3D11_USAGE_DEFAULT;
	consantBuffer.ByteWidth = sizeof(cbPerObject);
	consantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	consantBuffer.CPUAccessFlags = 0;
	consantBuffer.MiscFlags = 0;

	Device->CreateBuffer(&consantBuffer, NULL, &cbPerObjectBuffer);

	//Camera
	camPosition = DirectX::XMVectorSet(10.0f, 25, -17.0f, 0.0f);
	camTarget = DirectX::XMVectorSet(0.0f, -10.0f, 0.0f, 0.0f);
	camUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	//View 
	camView = DirectX::XMMatrixLookAtLH(camPosition, camTarget, camUp);

	//Projection 
	camProjection = DirectX::XMMatrixPerspectiveFovLH(0.4f * DirectX::XM_PI, (float)800 / 500, 0.01f, 1000.0f);
	

}
