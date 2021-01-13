//======================================================================
//											PostProcessing.cpp
//	ポストエフェクト
//
//======================================================================
//	author :	AT12A 05 宇佐美晃之
//======================================================================
//	開発履歴
//
//	2021/01/13	作成
//
//======================================================================


//===== インクルード部 =====
#include "PostProcessing.h"
#include "../System/Shader.h"
#include "../System/polygon.h"
#include "../System/Texture.h"


//===== マクロ定義 =====


//===== 構造体 =====
// 頂点シェーダー
struct VERTEX_SHADER_GLOBAL {
	XMMATRIX	mWorld;		// ワールド変換行列(転置行列)
	XMMATRIX	mView;		// ビュー変換行列(転置行列)
	XMMATRIX	mProj;		// 射影変換行列(転置行列)
	XMMATRIX	mTex;		// テクスチャ変換行列(転置行列)
};
// ピクセルシェーダー
struct PIXEL_SHADER_GLOBAL
{
	XMFLOAT4 color;

};


//===== プロトタイプ宣言 =====


//===== グローバル変数 =====


//===================================
//
//	コンストラクタ
//
//===================================
PostProcessing::PostProcessing()
{
}

//===================================
//
//	デストラクタ
//
//===================================
PostProcessing::~PostProcessing()
{
}

//===================================
//
//	初期化処理
//
//===================================
HRESULT PostProcessing::Init()
{
	ID3D11Device* pDevice = GetDevice();
	HRESULT hr;

	//// シェーダ初期化
	//static const D3D11_INPUT_ELEMENT_DESC layout[] = {
	//	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0},
	//	{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	//	{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	//	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	//};
	//hr = LoadShader("PostVertex", "PostPixel", &m_pVertexShader, &m_pInputLayout, &m_pPixelShader, layout, _countof(layout));
	//if (FAILED(hr)) {
	//	return hr;
	//}

	// 定数バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX_SHADER_GLOBAL);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = pDevice->CreateBuffer(&bd, nullptr, &m_pVertexConstantBuffer);
	if (FAILED(hr)) return hr;
	bd.ByteWidth = sizeof(PIXEL_SHADER_GLOBAL);
	hr = pDevice->CreateBuffer(&bd, nullptr, &m_pPixelConstantBuffer);
	if (FAILED(hr)) return hr;

	// テクスチャ サンプラ生成
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	hr = pDevice->CreateSamplerState(&sd, &m_pSamplerState);
	if (FAILED(hr)) {
		return hr;
	}

	// レンダーテクスチャ
	D3D11_TEXTURE2D_DESC renderTexture;
	ZeroMemory(&renderTexture, sizeof(D3D11_TEXTURE2D_DESC));
	//renderTexture.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	renderTexture.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	renderTexture.MipLevels = 1;
	renderTexture.ArraySize = 1;
	renderTexture.SampleDesc.Count = 1;
	renderTexture.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	renderTexture.Height = static_cast<UINT>(SCREEN_HEIGHT);
	renderTexture.Width = static_cast<UINT>(SCREEN_WIDTH);

	hr = pDevice->CreateTexture2D(
		&renderTexture,
		nullptr,
		&m_pRenderTexture
	);

	// レンダーターゲットビュー
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetView;
	ZeroMemory(&renderTargetView, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	renderTargetView.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	renderTargetView.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	hr = pDevice->CreateRenderTargetView(
		m_pRenderTexture,
		&renderTargetView,
		&m_pRenderTargetView
	);

	// レンダーテクスチャのシェーダーリソース
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	shaderResourceViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	hr = pDevice->CreateShaderResourceView(
		m_pRenderTexture,
		NULL,
		&m_pTexureResourceView
	);

	//ID3D11Resource* pResource = NULL;
	//m_pRenderTargetView->GetResource(&pResource);

	//hr = pDevice->CreateShaderResourceView(
	//	pResource,
	//	NULL,
	//	&m_pTexureResourceView
	//);

	return hr;
}

//===================================
//
//	終了処理
//
//===================================
HRESULT PostProcessing::Uninit()
{
	return S_OK;
}

//===================================
//
//	描画処理
//
//===================================
HRESULT PostProcessing::Draw()
{
	return S_OK;
}
