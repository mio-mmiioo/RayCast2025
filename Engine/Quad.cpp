#include "Quad.h"
#include "Camera.h"
#include "Texture.h"

Quad::Quad()
	:pVertexBuffer_(nullptr),
	pIndexBuffer_(nullptr),
	pConstantBuffer_(nullptr),
	pTexture_(nullptr)
{
}

Quad::~Quad()
{
	SAFE_RELEASE(pConstantBuffer_);
	SAFE_RELEASE(pIndexBuffer_);
	SAFE_RELEASE(pVertexBuffer_);
}

HRESULT  Quad::Initialize()
{
	VERTEX vertices[] =
	{
		{ XMVectorSet(-1.0f,  1.0f, 0.0f, 1.0f),XMFLOAT2(0.0f, 0.0f) },   // 四角形の頂点（左上）
		{ XMVectorSet(1.0f,  1.0f, 0.0f, 1.0f),	XMFLOAT2(1.0f, 0.0f) },   // 四角形の頂点（右上）
		{ XMVectorSet(1.0f, -1.0f, 0.0f, 1.0f),	XMFLOAT2(1.0f, 1.0f) },   // 四角形の頂点（右下）
		{ XMVectorSet(-1.0f, -1.0f, 0.0f, 1.0f),XMFLOAT2(0.0f, 1.0f) },   // 四角形の頂点（左下）
		//{ XMVectorSet(-1.0f, 1.0f, -1.0f, 0.0f), XMVectorSet(0.0f,  0.0f, 0.0f, 0.0f), XMVectorSet(●,▲,■, 0.0f) },   // 四角形の頂点（左上）
		//↑これで影つける
	};

	// 頂点データ用バッファの設定
	D3D11_BUFFER_DESC bd_vertex;
	bd_vertex.ByteWidth = sizeof(vertices);
	bd_vertex.Usage = D3D11_USAGE_DEFAULT;
	bd_vertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd_vertex.CPUAccessFlags = 0;
	bd_vertex.MiscFlags = 0;
	bd_vertex.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA data_vertex;
	data_vertex.pSysMem = vertices;
	if (FAILED(Direct3D::pDevice->CreateBuffer(&bd_vertex, &data_vertex, &pVertexBuffer_)))
	{
		MessageBox(nullptr, "頂点バッファの作成に失敗しました", "エラー", MB_OK);
		return E_FAIL;
	}

	//インデックス情報
	int index[] = { 0,2,3, 0,1,2 };

	// インデックスバッファを生成する
	D3D11_BUFFER_DESC   bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(index);
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = index;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	if (FAILED(Direct3D::pDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer_)))
	{
		MessageBox(nullptr, "インデックスバッファの作成に失敗しました", "エラー", MB_OK);
		return E_FAIL;
	}


	//コンスタントバッファ作成
	D3D11_BUFFER_DESC cb;
	cb.ByteWidth = sizeof(CONSTANT_BUFFER);
	cb.Usage = D3D11_USAGE_DYNAMIC;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	if (FAILED(Direct3D::pDevice->CreateBuffer(&cb, nullptr, &pConstantBuffer_)))
	{
		MessageBox(nullptr, "コンスタントバッファの作成に失敗しました", "エラー", MB_OK);
		return E_FAIL;
	}

	pTexture_ = new Texture();
	pTexture_->Load("Assets/dice.png");
	if (FAILED(pTexture_->Load("Assets/dice.png")))
	{
		MessageBox(nullptr, "テクスチャのロードに失敗しました", "エラー", MB_OK);
		return E_FAIL;
	}
	return S_OK;
}

void Quad::Draw(XMMATRIX& worldMatrix)
{
	//コンスタントバッファに渡す情報
	D3D11_MAPPED_SUBRESOURCE pdata;
	CONSTANT_BUFFER cb;
	cb.matWVP = XMMatrixTranspose(worldMatrix * Camera::GetViewMatrix() * Camera::GetProjectionMatrix());
	//cb.matW = 

	Direct3D::pContext->Map(pConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);	// GPUからのデータアクセスを止める
	memcpy_s(pdata.pData, pdata.RowPitch, (void*)(&cb), sizeof(cb));	// データを値を送る
	
	ID3D11SamplerState* pSampler = pTexture_->GetSampler();
	Direct3D::pContext->PSSetSamplers(0, 1, &pSampler);
	ID3D11ShaderResourceView* pSRV = pTexture_->GetSRV();
	Direct3D::pContext->PSSetShaderResources(0, 1, &pSRV);
	
	Direct3D::pContext->Unmap(pConstantBuffer_, 0);	//再開

	//頂点バッファ
	//UINT stride = sizeof(XMVECTOR);
	UINT stride = sizeof(VERTEX);//AIの予想
	UINT offset = 0;
	Direct3D::pContext->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);

	// インデックスバッファーをセット
	stride = sizeof(int);
	offset = 0;
	Direct3D::pContext->IASetIndexBuffer(pIndexBuffer_, DXGI_FORMAT_R32_UINT, 0);

	//コンスタントバッファ
	Direct3D::pContext->VSSetConstantBuffers(0, 1, &pConstantBuffer_);	//頂点シェーダー用	
	Direct3D::pContext->PSSetConstantBuffers(0, 1, &pConstantBuffer_);	//ピクセルシェーダー用

	Direct3D::pContext->DrawIndexed(12, 0, 0);
}

void Quad::Release()
{
	SAFE_RELEASE(pTexture_);
	SAFE_DELETE(pTexture_);

	SAFE_RELEASE(pConstantBuffer_);
	SAFE_RELEASE(pIndexBuffer_);
	SAFE_RELEASE(pVertexBuffer_);
}
