#pragma once
#include "DX11Renderer.h"
#include <vector>

struct Vertex
{
	float x, y, z; // pos
	float r, g, b, a; //color
};

class Triangle
{
public:
	Triangle(DX11Renderer& renderer, float xpos, float ypos, float zpos, float rSpeed);
	~Triangle();

	void CreateShaders(DX11Renderer& renderer);
	void Draw(DX11Renderer& renderer);
	void Update();

	void SetNewPos(float x, float y, float z, bool changeY);
	void ChangeColorRandom();

	void TempSetPosMatrix(DirectX::XMVECTOR X)
	{
		Translation = DirectX::XMMatrixTranslationFromVector(X);
	};

	std::vector<DirectX::XMFLOAT3> getVericies()
	{
		return verticiesArray;
	};

	std::vector<unsigned short> getIndicies()
	{
		return indicesArray;
	};
	
	DirectX::XMMATRIX getObjWorld()
	{
		return world;
	}

	void hideObj()
	{
		hide = true;
	}

	float getX() { return x; };
	float getY() { return y; };
	float getZ() { return z; };

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
	float rotSpeed;

	std::vector<unsigned short> indicesArray;

	std::vector<DirectX::XMFLOAT3> verticiesArray;

	bool hide;

	float x;
	float y;
	float z;

};
