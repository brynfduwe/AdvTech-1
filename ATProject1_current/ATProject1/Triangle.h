#pragma once
#include "DX11Renderer.h"

class Triangle
{
public:
	Triangle(DX11Renderer& renderer, float xpos, float ypos, float zpos);
	~Triangle();

	void CreateShaders(DX11Renderer& renderer);
	void Draw(DX11Renderer& renderer);
	void Update();

	void ChangeColorRandom();

private:

	ID3D11Buffer * vertexBuffer;
	ID3D11VertexShader * vertexShader = nullptr;
	ID3D11PixelShader * pixelShader = nullptr;

	ID3D11InputLayout * inputLayout = nullptr;

	ID3D11Buffer* squareIndexBuffer;
	ID3D11Buffer* squareVertBuffer;

	ID3D11Buffer* WVP_buffer = nullptr;
	DirectX::XMMATRIX WVP;

	DirectX::XMMATRIX world;

	DirectX::XMMATRIX Rotation;
	DirectX::XMMATRIX Scale;
	DirectX::XMMATRIX Translation;
	float rotation = 0.01f;
};
