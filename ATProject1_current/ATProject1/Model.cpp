#include "Model.h"
#include <fstream>
#include <vector>
#include <ctime>

Model::Model(DX11Renderer & renderer, float xpos, float ypos, float zpos, float rSpeed)
{
	standingSpace = -1;

	selected = false;
	highlighted = false;

	Translation = DirectX::XMMatrixTranslation(xpos, ypos, zpos);
	x = xpos;
	y = ypos;
	z = zpos;

	//window
	//-1, 1   0, 1,   1, 1
	// 0,-1   0, 0,   1, 0
	//-1,-1   0,-1,   1,-1

	//most basic of geometry
	Vertex verticies[] =
	{
	{ -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f },
	{ -1.0f,  +1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f },
	{ +1.0f,  +1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f },
	{ +1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f },
	{ -1.0f, -1.0f,  +1.0f, 0.0f, 1.0f, 0.0f, 1.0f },
	{ -1.0f,  +1.0f, +1.0f, 1.0f, 0.0f, 0.0f, 1.0f },
	{ +1.0f,  +1.0f,  +1.0f, 0.0f, 1.0f, 0.0f, 1.0f },
	{ +1.0f, -1.0f,  +1.0f, 0.0f, 0.0f, 1.0f, 1.0f },
	};

	for (int i = 0; i < 8; i++)
	{
		DirectX::XMFLOAT3 V;
		V.x = verticies[i].x;
		V.y = verticies[i].y;
		V.z = verticies[i].z;
		verticiesArray.push_back(V);
	}


	unsigned short indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	for (int i = 0; i < 36; i++)
	{
		indicesArray.push_back(indices[i]);
	}

//	ChangeColorRandom();

	//INDEX

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * 12* 3 ;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;

	iinitData.pSysMem = indices;
	renderer.getDevice()->CreateBuffer(&indexBufferDesc, &iinitData, &squareIndexBuffer);

	renderer.getDeviceContex()->IASetIndexBuffer(squareIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * 8;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = verticies;
	renderer.getDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &squareVertBuffer);

	//vertex buffer
	vertexBufferDesc = CD3D11_BUFFER_DESC(sizeof(verticies), D3D11_BIND_VERTEX_BUFFER);
	D3D11_SUBRESOURCE_DATA vertexdata = { 0 };
	vertexdata.pSysMem = verticies;

	renderer.getDevice()->CreateBuffer(&vertexBufferDesc, &vertexdata, &vertexBuffer);

	renderer.getDeviceContex()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	auto WVP_desc = CD3D11_BUFFER_DESC(sizeof(renderer.getWVP()), D3D11_BIND_CONSTANT_BUFFER);

	renderer.getDevice()->CreateBuffer(&WVP_desc, nullptr, &WVP_buffer);

	CreateShaders(renderer);

	rotSpeed = rSpeed;

}

Model::~Model()
{
	//vertexBuffer->Release();
	//vertexShader->Release();
	//pixelShader->Release();
	//inputLayout->Release();
	//squareIndexBuffer->Release();
	//squareVertBuffer->Release();
	//WVP_buffer->Release();
}

void Model::CreateShaders(DX11Renderer & renderer)
{
	std::ifstream vs("TriangleVertexShader.cso", std::ios::binary);
	std::ifstream ps("TrianglePixelShader.cso", std::ios::binary);
	std::vector<char> vsData = { std::istreambuf_iterator<char>(vs), std::istreambuf_iterator<char>() };
	std::vector<char> psData = { std::istreambuf_iterator<char>(ps), std::istreambuf_iterator<char>() };

	//creates shader with dx11
	renderer.getDevice()->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &vertexShader);
	renderer.getDevice()->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader);

	//input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	renderer.getDevice()->CreateInputLayout(layout, ARRAYSIZE(layout), vsData.data(), vsData.size(), &inputLayout);
}

void Model::Draw(DX11Renderer & renderer)
{
	if (!hide)
	{
		///RENDER
		WVP = renderer.getWVP();

		renderer.getDeviceContex()->UpdateSubresource(WVP_buffer, 0, nullptr, &WVP, 0, 0);

		renderer.getDeviceContex()->ClearDepthStencilView(renderer.getDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);

		renderer.setRenderTargets();

		renderer.getDeviceContex()->IASetInputLayout(inputLayout);
		renderer.getDeviceContex()->VSSetShader(vertexShader, nullptr, 0);
		renderer.getDeviceContex()->PSSetShader(pixelShader, nullptr, 0);

		//bind vertex buffer - bind saves memory by allowing re-rendering
		UINT vStride = sizeof(Vertex);
		UINT offset = 0;
		renderer.getDeviceContex()->IASetVertexBuffers(0, 1, &vertexBuffer, &vStride, &offset);

		//input assembler
		renderer.getDeviceContex()->IASetIndexBuffer(squareIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
		renderer.getDeviceContex()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		renderer.getDeviceContex()->VSSetConstantBuffers(0, 1, &WVP_buffer);

		renderer.setObjWVP(world);

		renderer.getDeviceContex()->DrawIndexed(36, 0, 0);
		//renderer.getDeviceContex()->Draw(8, 0);
	}

}


void Model::Update()
{
	rotation += rotSpeed;
	if (rotation > 6.26f)
		rotation = 0.0f;

	world = DirectX::XMMatrixIdentity();

	DirectX::XMVECTOR rotaxis = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	Rotation = DirectX::XMMatrixRotationAxis(rotaxis, rotation);
	//Translation = DirectX::XMMatrixTranslation(0.0f, 0.0f, 4.0f);

	//lerp movement
	if (moving)
	{
		DirectX::XMVECTOR pos = DirectX::XMVectorSet(0, 0, 0, 0);
		pos = DirectX::XMVector3Transform(pos, Translation);
	//	pos = DirectX::XMVectorLerp(pos, DirectX::XMVectorSet(moveToPos.x, moveToPos.y, moveToPos.z, 0), 0.1f);

		pos = DirectX::XMVectorLerp(pos, DirectX::XMVectorSet(pathList[pathIter].x, y, pathList[pathIter].z, 0), 0.15f);
		Translation = DirectX::XMMatrixTranslationFromVector(pos);

		lerpTimer += 0.1f;
		if (lerpTimer > 1)
		{
			if (pathIter < pathList.size() - 1)
			{
				x = pathList[pathIter].x;
				z = pathList[pathIter].z;

				//Translation = DirectX::XMMatrixTranslation(pathList[pathIter].x, y, pathList[pathIter].z);
				pathIter++;
				lerpTimer = 0;
			}
			else
			{
				moving = false;
			}
		}
	}

	world = Translation * Rotation;
}

void Model::SetNewPos(float xPos, float yPos, float zPos, bool changeY)
{
	moveToPos.x = xPos;
	if(changeY)
		moveToPos.y = yPos;
	moveToPos.z = zPos;

	moving = true;
//	Translation = DirectX::XMMatrixTranslation(x, y, z);
}

void Model::SetPathList(std::vector<DirectX::XMFLOAT3> positionList)
{
	pathList.clear();
	pathList = positionList;
	currentMoveTo = pathList[0];
	pathIter = 0;
	moving = true;
}

void Model::HighlightObject(DX11Renderer & renderer)
{
	if (highlighted == false && selected == false)
	{
		highlighted = true;

		verticiesArray.clear();

		Vertex verticies[] =
		{
		{ -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f },
		{ -1.0f,  +1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f },
		{ +1.0f,  +1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f },
		{ +1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f },
		{ -1.0f, -1.0f,  +1.0f, 0.0f, 1.0f, 1.0f, 1.0f },
		{ -1.0f,  +1.0f, +1.0f, 1.0f, 0.0f, 1.0f, 1.0f },
		{ +1.0f,  +1.0f,  +1.0f, 0.0f, 1.0f, 0.0f, 1.0f },
		{ +1.0f, -1.0f,  +1.0f, 1.0f, 0.0f, 1.0f, 1.0f },
		};

		for (int i = 0; i < 8; i++)
		{
			DirectX::XMFLOAT3 V;
			V.x = verticies[i].x;
			V.y = verticies[i].y;
			V.z = verticies[i].z;
			verticiesArray.push_back(V);
		}

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(Vertex) * 8;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;

		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = verticies;
		renderer.getDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &squareVertBuffer);

		//vertex buffer
		vertexBufferDesc = CD3D11_BUFFER_DESC(sizeof(verticies), D3D11_BIND_VERTEX_BUFFER);
		D3D11_SUBRESOURCE_DATA vertexdata = { 0 };
		vertexdata.pSysMem = verticies;

		renderer.getDevice()->CreateBuffer(&vertexBufferDesc, &vertexdata, &vertexBuffer);

		renderer.getDeviceContex()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		auto WVP_desc = CD3D11_BUFFER_DESC(sizeof(renderer.getWVP()), D3D11_BIND_CONSTANT_BUFFER);

		renderer.getDevice()->CreateBuffer(&WVP_desc, nullptr, &WVP_buffer);

		CreateShaders(renderer);
	}
}



void Model::UnHighlightObject(DX11Renderer & renderer)
{
	if (highlighted == true && selected == false)
	{
		highlighted = false;

		verticiesArray.clear();

		Vertex verticies[] =
		{
			{ -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f },
		{ -1.0f,  +1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f },
		{ +1.0f,  +1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f },
		{ +1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f },
		{ -1.0f, -1.0f,  +1.0f, 0.0f, 1.0f, 0.0f, 1.0f },
		{ -1.0f,  +1.0f, +1.0f, 1.0f, 0.0f, 0.0f, 1.0f },
		{ +1.0f,  +1.0f,  +1.0f, 0.0f, 1.0f, 0.0f, 1.0f },
		{ +1.0f, -1.0f,  +1.0f, 0.0f, 0.0f, 1.0f, 1.0f },
		};
		for (int i = 0; i < 8; i++)
		{
			DirectX::XMFLOAT3 V;
			V.x = verticies[i].x;
			V.y = verticies[i].y;
			V.z = verticies[i].z;
			verticiesArray.push_back(V);
		}

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(Vertex) * 8;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;

		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = verticies;
		renderer.getDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &squareVertBuffer);

		//vertex buffer
		vertexBufferDesc = CD3D11_BUFFER_DESC(sizeof(verticies), D3D11_BIND_VERTEX_BUFFER);
		D3D11_SUBRESOURCE_DATA vertexdata = { 0 };
		vertexdata.pSysMem = verticies;

		renderer.getDevice()->CreateBuffer(&vertexBufferDesc, &vertexdata, &vertexBuffer);

		renderer.getDeviceContex()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		auto WVP_desc = CD3D11_BUFFER_DESC(sizeof(renderer.getWVP()), D3D11_BIND_CONSTANT_BUFFER);

		renderer.getDevice()->CreateBuffer(&WVP_desc, nullptr, &WVP_buffer);

		CreateShaders(renderer);
	}
}
