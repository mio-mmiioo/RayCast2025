#pragma once

#include <d3d11.h>
#include <fbxsdk.h>
#include <string>
#include "Transform.h"
#include "Texture.h"
#include <vector>

#pragma comment(lib, "LibFbxSDK-MD.lib")
#pragma comment(lib, "LibXml2-MD.lib")
#pragma comment(lib, "zlib-MD.lib")

namespace Math
{
	float Det(XMFLOAT3 a, XMFLOAT3 b, XMFLOAT3 c);
	bool InterSects(XMVECTOR vOrigin, XMVECTOR vRay, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2);
}

// RayCastのためのデータを用意
struct RayCastData
{
	XMFLOAT4 start; // Rayの始点
	XMFLOAT4 direction; // Rayの方向（正規化してあること）
	bool isHit; // 当たったかどうか
	float distance; // 始点からの距離
};

class Fbx
{
	//マテリアル
	struct MATERIAL
	{
		Texture* pTexture;
		XMFLOAT4 diffuse;
	};

	struct CONSTANT_BUFFER
	{
		XMMATRIX	matWVP;
		XMMATRIX	matNormal;
		XMFLOAT4	diffuse;
		BOOL		materialFlag; //マテリアルがあるかないか
	};
	struct VERTEX
	{
		XMVECTOR position;
		XMVECTOR uv;
		XMVECTOR normal;
	};

	ID3D11Buffer* pVertexBuffer_;
	ID3D11Buffer** pIndexBuffer_;
	ID3D11Buffer* pConstantBuffer_;
	std::vector<MATERIAL> pMaterialList_;
	std::vector<int> indexCount_;//マテリアルごとのインデックス数

	int vertexCount_;	//頂点数
	int polygonCount_;	//ポリゴン数
	int materialCount_;	//マテリアルの個数

	std::vector<VERTEX> vertices_; // 頂点データ全部
	std::vector<std::vector<int>> indicesPerMat_; // マテリアルごとのインデックスデータ[material][index]
	

public:

	Fbx();
	HRESULT Load(std::string fileName);
	void    Draw(Transform& transform);
	void    Release();
	void RayCast(RayCastData& rayData);

	void InitVertex(FbxMesh* mesh);
	void InitIndex(FbxMesh* mesh);
	void InitConstantBuffer();
	void InitMaterial(fbxsdk::FbxNode* pNode);
};