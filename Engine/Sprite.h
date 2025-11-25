#pragma once
#include "Direct3D.h"
#include <DirectXMath.h>
//#include "Texture.h"

using namespace DirectX;

//コンスタントバッファー
struct CONSTANT_BUFFER
{
	//XMMATRIX	matWVP;
	XMMATRIX	matW;//ワールド行列
};

//頂点情報
struct VERTEX
{
	XMFLOAT4 position;
	XMFLOAT2 uv;
};

class Texture;

class Sprite
{
	ID3D11Buffer* pVertexBuffer_;	//頂点バッファ
	ID3D11Buffer* pIndexBuffer_;	//インデックスバッファ
	ID3D11Buffer* pConstantBuffer_;	//コンスタントバッファ

	Texture* pTexture_;

public:
	Sprite();
	~Sprite();
	HRESULT Initialize();
	void Draw(XMMATRIX& worldMatrix);
	void Release();
};
