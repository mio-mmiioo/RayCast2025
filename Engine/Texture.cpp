#include <wincodec.h>

#include "Texture.h"
#include <DirectXTex.h>
#include "Direct3D.h"

// DirectXTexのライブラリをリンク

#pragma comment(lib, "DirectXTex.lib")


Texture::Texture()
{
}

Texture::~Texture()
{
}

HRESULT Texture::Load(std::string fileName)
{
	DirectX::TexMetadata metadata; //画像の付属情報

	DirectX::ScratchImage image;   //画像本体

	HRESULT hr;

	//実際に読んでゆくぅ　　　　　 
	std::wstring wfileName(fileName.begin(), fileName.end());
	hr = LoadFromWICFile(wfileName.c_str(), DirectX::WIC_FLAGS::WIC_FLAGS_NONE, &metadata, image);

	if (FAILED(hr))
	{
		MessageBox(nullptr, "Texture", "エラー", MB_OK);
		return S_FALSE;
	}

	D3D11_SAMPLER_DESC  SamDesc;
	ZeroMemory(&SamDesc, sizeof(D3D11_SAMPLER_DESC));
	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	hr = Direct3D::pDevice->CreateSamplerState(&SamDesc, &pSampler_);
	if (FAILED(hr))
	{
		MessageBox(nullptr, "Texture:サンプラーステートの作成に失敗しました", "エラー", MB_OK);
		return S_FALSE;
	}

	//シェーダーリソースビュー
	D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
	//srv.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srv.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv.Texture2D.MipLevels = 1;
	hr = CreateShaderResourceView(Direct3D::pDevice,image.GetImages(), image.GetImageCount(), metadata, &pSRV_);
	if (FAILED(hr))
	{
		MessageBox(nullptr, "Texture:シェーダーリソースビュー", "エラー", MB_OK);
		return S_FALSE;
	}

	return S_OK;
}

void Texture::Release()
{
	SAFE_RELEASE(pSRV_);
	SAFE_RELEASE(pSampler_);
}
