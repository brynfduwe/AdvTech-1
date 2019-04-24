#pragma once
#include "DX11Renderer.h"
#include <vector>

struct Vertex
{
	float x, y, z; // pos
	float r, g, b, a; //color
};

class Model
{
public:
	Model(DX11Renderer& renderer, float xpos, float ypos, float zpos, float rSpeed);
	~Model();

	void CreateShaders(DX11Renderer& renderer);
	void Draw(DX11Renderer& renderer);
	void Update();

	void SetNewPos(float x, float y, float z, bool changeY);

	void SetPathList(std::vector<DirectX::XMFLOAT3> positionList);

	void SetSelect(bool select)
	{
		selected = select;
	};

	void HighlightObject(DX11Renderer& renderer);
	void UnHighlightObject(DX11Renderer & renderer);

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

	void setStandingSpace(int set) { standingSpace = set; }
	int getStandingSpace() { return standingSpace; }

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

	DirectX::XMFLOAT3 moveToPos;
	bool moving;
	float lerpTimer;

	std::vector<DirectX::XMFLOAT3> pathList;
	DirectX::XMFLOAT3 currentMoveTo;
	int pathIter;

	int standingSpace;

	bool highlighted;
	bool selected;
};