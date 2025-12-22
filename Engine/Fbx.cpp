#include "Fbx.h"
#include "Direct3D.h"
#include "Camera.h"
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

Fbx::Fbx()
	: pVertexBuffer_(nullptr)
	, pIndexBuffer_(nullptr)
	, pConstantBuffer_(nullptr)
	, vertexCount_(0)
	, polygonCount_(0)
	, materialCount_(0)
{
}

HRESULT Fbx::Load(std::string fileName)
{
	using std::string;
	string subDir("Assets");
	fs::path currPath, basePath;
	currPath = fs::current_path();
	basePath = currPath;
	currPath = currPath / subDir;
	//fs::path subPath(currPath.string() + "\\" + subDir);
	assert(fs::exists(currPath));//subPathはあります、という確認
	fs::current_path(currPath);

	//マネージャを生成
	FbxManager* pFbxManager = FbxManager::Create();

	//インポーターを生成
	FbxImporter* fbxImporter = FbxImporter::Create(pFbxManager, "imp");
	fbxImporter->Initialize(fileName.c_str(), -1, pFbxManager->GetIOSettings());

	//シーンオブジェクトにFBXファイルの情報を流し込む
	FbxScene* pFbxScene = FbxScene::Create(pFbxManager, "fbxscene");
	fbxImporter->Import(pFbxScene);
	fbxImporter->Destroy();

	//メッシュ情報を取得
	FbxNode* rootNode = pFbxScene->GetRootNode();
	FbxNode* pNode = rootNode->GetChild(0);
	FbxMesh* mesh = pNode->GetMesh();

	//各情報の個数を取得
	vertexCount_ = mesh->GetControlPointsCount();	//頂点の数
	polygonCount_ = mesh->GetPolygonCount();		//ポリゴンの数
	materialCount_ = pNode->GetMaterialCount();		//マテリアルの数

	InitVertex(mesh);
	InitIndex(mesh);
	InitConstantBuffer();
	InitMaterial(pNode);

	fs::current_path(basePath);

	//マネージャ解放
	pFbxManager->Destroy();
	return S_OK;
}

void Fbx::Draw(Transform& transform)
{
	//Quadをアレンジ
	Direct3D::SetShader(SHADER_3D);
	transform.Calculation();

	CONSTANT_BUFFER cb;
	cb.matWVP = XMMatrixTranspose(transform.GetWorldMatrix() * Camera::GetViewMatrix() * Camera::GetProjectionMatrix());
	cb.matNormal = transform.GetNormalMatrix();

	//頂点バッファ、インデックスバッファ、コンスタントバッファをパイプラインにセット
	//頂点バッファ
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	Direct3D::pContext->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);

	for (int i = 0; i < materialCount_; i++)
	{
		if (pMaterialList_[i].pTexture)
		{
			cb.materialFlag = TRUE;
			cb.diffuse = XMFLOAT4(1, 1, 1, 1);//保険
		}
		else
		{
			cb.materialFlag = FALSE;
			cb.diffuse = pMaterialList_[i].diffuse;
		}

		D3D11_MAPPED_SUBRESOURCE pdata;
		Direct3D::pContext->Map(pConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);	// GPUからのデータアクセスを止める
		memcpy_s(pdata.pData, pdata.RowPitch, (void*)(&cb), sizeof(cb));	// データを値を送る

		Direct3D::pContext->Unmap(pConstantBuffer_, 0);	//再開

		// インデックスバッファーをセット
		stride = sizeof(int);
		offset = 0;
		Direct3D::pContext->IASetIndexBuffer(pIndexBuffer_[i], DXGI_FORMAT_R32_UINT, 0);

		//コンスタントバッファ
		Direct3D::pContext->VSSetConstantBuffers(0, 1, &pConstantBuffer_);	//頂点シェーダー用	
		Direct3D::pContext->PSSetConstantBuffers(0, 1, &pConstantBuffer_);	//ピクセルシェーダー用

		if (pMaterialList_[i].pTexture)
		{
			ID3D11SamplerState* pSampler = pMaterialList_[i].pTexture->GetSampler();
			Direct3D::pContext->PSSetSamplers(0, 1, &pSampler);

			ID3D11ShaderResourceView* pSRV = pMaterialList_[i].pTexture->GetSRV();
			Direct3D::pContext->PSSetShaderResources(0, 1, &pSRV);
		}

		//描画
		Direct3D::pContext->DrawIndexed(indexCount_[i], 0, 0);
	}
}

void Fbx::Release()
{
}

void Fbx::InitVertex(FbxMesh* mesh)
{
	vertices_.resize(vertexCount_);
	// 全ポリゴン
	for (long poly = 0; poly < polygonCount_; poly++)
	{
		// 3頂点分
		for (int vertex = 0; vertex < 3; vertex++)
		{
			//調べる頂点の番号
			int index = mesh->GetPolygonVertex(poly, vertex);

			//頂点の位置
			FbxVector4 pos = mesh->GetControlPointAt(index);
			vertices_[index].position = XMVectorSet((float)pos[0], (float)pos[1], (float)pos[2], 0.0f);

			//頂点のUV
			FbxLayerElementUV* pUV = mesh->GetLayer(0)->GetUVs();
			int uvIndex = mesh->GetTextureUVIndex(poly, vertex, FbxLayerElement::eTextureDiffuse);
			FbxVector2  uv = pUV->GetDirectArray().GetAt(uvIndex);
			vertices_[index].uv = XMVectorSet((float)uv.mData[0], (float)(1.0f - uv.mData[1]), 0.0f, 0.0f);

			//頂点の法線
			FbxVector4 normal;
			mesh->GetPolygonVertexNormal(poly, vertex, normal);
			vertices_[index].normal = XMVectorSet((float)normal[0], (float)normal[1], (float)normal[2], 0.0f);
		}
	}

	// 頂点バッファ作成
	HRESULT hr;
	D3D11_BUFFER_DESC bd_vertex;
	bd_vertex.ByteWidth = sizeof(VERTEX) * vertexCount_;
	bd_vertex.Usage = D3D11_USAGE_DEFAULT;
	bd_vertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd_vertex.CPUAccessFlags = 0;
	bd_vertex.MiscFlags = 0;
	bd_vertex.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA data_vertex;
	//data_vertex.pSysMem = vertices;
	data_vertex.pSysMem = vertices_.data();
	hr = Direct3D::pDevice->CreateBuffer(&bd_vertex, &data_vertex, &pVertexBuffer_);
	if (FAILED(hr))
	{
		MessageBox(NULL, "頂点バッファの作成に失敗しました", "エラー", MB_OK);
	}


}

void Fbx::InitIndex(FbxMesh* mesh)
{
	pIndexBuffer_ = new ID3D11Buffer * [materialCount_];

	indicesPerMat_.resize(materialCount_);
	indexCount_ = std::vector<int>(materialCount_);
	
	for (int i = 0; i < materialCount_; i++)
	{
		auto& indeces = indicesPerMat_[i];

		//全ポリゴン
		for (long poly = 0; poly < polygonCount_; poly++)
		{
			FbxLayerElementMaterial* mtl = mesh->GetLayer(0)->GetMaterials();
			int mtlId = mtl->GetIndexArray().GetAt(poly);

			if (mtlId == i)
			{
				for (long vertex = 0; vertex < 3; vertex++)
				{
					indeces.push_back(mesh->GetPolygonVertex(poly, vertex));
				}
			}
		}

		indexCount_[i] = (int)indeces.size();

		D3D11_BUFFER_DESC   bd;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(int) * indexCount_[i];
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = indeces.data();
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		HRESULT hr;
		hr = Direct3D::pDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer_[i]);
		if (FAILED(hr))
		{
			MessageBox(NULL, "インデックスバッファの作成に失敗しました", "エラー", MB_OK);
		}
	}
}

void Fbx::InitConstantBuffer()
{
	//Quadと一緒
	D3D11_BUFFER_DESC cb;
	cb.ByteWidth = sizeof(CONSTANT_BUFFER);
	cb.Usage = D3D11_USAGE_DYNAMIC;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;

	// コンスタントバッファの作成
	HRESULT hr;
	hr = Direct3D::pDevice->CreateBuffer(&cb, nullptr, &pConstantBuffer_);
	if (FAILED(hr))
	{
		MessageBox(NULL, "コンスタントバッファの作成に失敗しました", "エラー", MB_OK);
	}
}

void Fbx::InitMaterial(fbxsdk::FbxNode* pNode)
{
	//materialCount_の数だけ配列を準備
	pMaterialList_.resize(materialCount_);
	for (int i = 0; i < materialCount_; i++)
	{
		FbxSurfaceMaterial* pMaterial = pNode->GetMaterial(i);//i番目のマテリアル情報を取得
		FbxProperty  lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);//テクスチャ情報
		int fileTextureCount = lProperty.GetSrcObjectCount<FbxFileTexture>();//テクスチャの数

		//テクスチャあり
		if (fileTextureCount > 0)
		{
			FbxFileTexture* textureInfo = lProperty.GetSrcObject<FbxFileTexture>(0);
			const char* textureFilePath = textureInfo->GetRelativeFileName();
			
			fs::path tPath(textureFilePath);
			if (fs::is_regular_file(tPath))
			{
				//ここでテクスチャの読み込み
			
				pMaterialList_[i].pTexture = new Texture;
				pMaterialList_[i].pTexture->Load(tPath.string());
			}
			else
			{
				//テクスチャファイルが無いときの処理(エラー）
				MessageBox(NULL, "テクスチャファイルが見つかりませんでした", "エラー", MB_OK);
			}

		}
		//テクスチャ無し
		else
		{
			//テスクチャないときの処理
			pMaterialList_[i].pTexture = nullptr;
			
			//マテリアルの色 　Lambert：拡散反射と、アンビエントのみのシェーディングモデル
			//FbxDouble3 color = ((FbxSurfaceLambert*)pMaterial)->Diffuse.Get();
			//pMaterialList_[i].diffuse = { (float)color[0], (float)color[1], (float)color[2], 1.0f };
			////pMaterialList_[i].diffuse = XMFLOAT4((float)color[0], (float)color[1], (float)color[2], 1.0f);

			FbxSurfaceLambert* pMaterial = (FbxSurfaceLambert*)pNode->GetMaterial(i);
			FbxDouble3  diffuse = pMaterial->Diffuse;
			pMaterialList_[i].diffuse = XMFLOAT4((float)diffuse[0], (float)diffuse[1], (float)diffuse[2], 1.0f);
		}

	}
}


void Fbx::RayCast(RayCastData& rayData)
{
	XMVECTOR start = XMLoadFloat4(&rayData.start);
	XMVECTOR dir = XMVector3Normalize(XMLoadFloat4(&rayData.direction));

	for (int material = 0; material < materialCount_; material++)
	{
		const auto& indices = indicesPerMat_[material];
		// 全ポリゴンに対して
		for (int i = 0; i < (int)indices.size(); i += 3)
		{
			const VERTEX& v0 = vertices_[indices[i + 0]];
			const VERTEX& v1 = vertices_[indices[i + 1]];
			const VERTEX& v2 = vertices_[indices[i + 2]];
			rayData.isHit = Math::InterSects(start, dir, v0.position, v1.position, v2.position);
			if (rayData.isHit)
			{
				return;
			}
		}
	}
	rayData.isHit = false;
}

float Math::Det(XMFLOAT3 a, XMFLOAT3 b, XMFLOAT3 c)
{
	float ret =
		(a.x + b.y + c.z) +
		(b.x + c.y + a.z) +
		(c.x + b.z + a.y) -
		(a.x + b.z + c.y) -
		(b.x + a.y + c.z) -
		(c.x + b.y + a.z);
	return ret;
}

bool Math::InterSects(XMVECTOR vOrigin, XMVECTOR vRay, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2)
{
	// Rayの始点・方向、三角形の頂点をXMVECTORに変換←DirectXMathで計算するため
	//XMVECTOR vOrigin = XMLoadFloat3(&origin);
	//XMVECTOR ray = XMLoadFloat3(&ray);
	//XMVECTOR v0 = XMLoadFloat3(&v0);
	//XMVECTOR v1 = XMLoadFloat3(&v1);
	//XMVECTOR v2 = XMLoadFloat3(&v2);

	// 三角形の2本の辺のベクトルを作る
	//XMVECTOR vEdge1 = XMVECTOR(v1.x - v0.x, v1.y - v0.y, v1.z - v0.z);
	//XMVECTOR vEdge2 = XMVECTOR(v2.x - v0.x, v2.y - v0.y, v2.z - v0.z);
	XMVECTOR vEdge1 = v1 - v0;
	XMVECTOR vEdge2 = v2 - v0;

	// 行列式計算のため、XMFLOAT3にもどす
	XMFLOAT3 edge1;
	XMFLOAT3 edge2;
	XMStoreFloat3(&edge1, vEdge1);
	XMStoreFloat3(&edge2, vEdge2);

	// 三角形の基準点v0から、Rayの開始点originへのベクトル
	// t*ray = (v0 - origin) + u*edge1 + v*edge2 をする準備
	XMFLOAT3 d;
	//XMVECTOR tmp = { origin.x - v0.x, origin.y - v0.y, origin.z - v0.z };
	XMVECTOR tmp = vOrigin - v0;
	XMStoreFloat3(&d, tmp);

	// u*edge1 + v*edge2 + t*(-ray) = d の形に合わせるために-1をかける
	/*ray = {
		ray.x * -1.0f,
		ray.y * -1.0f,
		ray.z * -1.0f
	};*/
	vRay = vRay * -1.0f;
	XMFLOAT3 ray;
	XMStoreFloat3(&ray, vRay);
	// denom = det(edge1, edge2, -ray)
	// →3本のベクトルが作る行列の計算式
	// 0なら、平面とRayが平行で交点を持たない
	float denom = Det(edge1, edge2, ray);

	// 平行の判定（解なし）
	if (denom <= 0.0f)
	{
		return false; // Rayが三角形の平面と交差しない
	}

	// クラメルの公式でu,v,tを求める
	float u = Det(d, edge2, ray) / denom; // 交点がedge1方向にどれだけ進んだか
	float v = Det(edge1, d, ray) / denom; // 交点がedge2方向にどれだけ進んだか
	float t = Det(edge1, edge2, d) / denom; // Rayの開始点から交点までの距離

	// 三角形内部 + Rayの向き 判定
	// u + v <= 1 → 三角形の内部に入っている
	if (u + v <= 1)
	{
		return true;
	}
	return false;
}
