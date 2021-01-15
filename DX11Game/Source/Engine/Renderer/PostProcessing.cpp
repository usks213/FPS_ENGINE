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
struct GaussBlurParam
{
	int		 nSampleCount;
	XMFLOAT4 Offset[16];
};


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
	XMMATRIX	mTex;			// テクスチャ変換行列(転置行列)
	XMVECTOR	vColor;			// カラー
	int			nSampleCount;	// サンプル回数(ループ)
	XMVECTOR	vOffset[16];	// ガウスブラーのオフセット
};


//===== プロトタイプ宣言 =====
//      ガウスの重みを計算します.
float GaussianDistribution(const XMFLOAT2& pos, float rho);
//      ブラーパラメータを計算します.
GaussBlurParam CalcBlurParam(int width, int height, XMFLOAT2 dir, float deviation);


//===== グローバル変数 =====
static GaussBlurParam g_gaussParam;
PostProcessing* PostProcessing::m_pMain = nullptr;



//===================================
//
//	コンストラクタ
//
//===================================
PostProcessing::PostProcessing()
{
	m_pVertexShader = NULL;		// 頂点シェーダ
	m_pPixelShader = NULL;			// ピクセルシェーダ
	m_pInputLayout = NULL;			// 頂点フォーマット

	m_pVertexConstantBuffer = NULL;		// 頂点シェーダー定数バッファ
	m_pPixelConstantBuffer = NULL;			// ピクセルシェーダー定数バッファ
	m_pSamplerState = NULL;				// テクスチャ サンプラ
	m_pVertexBuffer = NULL;				// 頂点バッファ

	m_pRenderTargetView = NULL;	// レンダーターゲット
	m_pRenderTexture = NULL;		// レンダーテクスチャ
	m_pTexureResourceView = NULL;	// テクスチャリソース

	// 変換行列初期化
	XMStoreFloat4x4(&m_mWorld, XMMatrixIdentity());
	XMStoreFloat4x4(&m_mView, XMMatrixLookAtLH(XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));
	XMStoreFloat4x4(&m_mProj,
		XMMatrixOrthographicLH(SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f, 100.0f));
	XMStoreFloat4x4(&m_mTex, XMMatrixIdentity());
	m_mTex._44 = 0.0f;

	m_pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_siz = XMFLOAT3(SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f);

	m_col = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	m_posTex = XMFLOAT2(0.0f, 0.0f);
	m_sizTex = XMFLOAT2(1.0f, 1.0f);


	// ガウスブラーのパラメータ
	g_gaussParam = CalcBlurParam(1.3f, 1.3f, XMFLOAT2{ 1,1 }, 1.0f);

	m_pMain = this;
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
HRESULT PostProcessing::Init(ID3D11Device* pDevice)
{
	HRESULT hr;

	// シェーダ初期化
	static const D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	hr = LoadShader("PostVertex2D", "PostPixel2D", &m_pVertexShader, &m_pInputLayout, &m_pPixelShader, layout, _countof(layout));
	if (FAILED(hr)) {
		return hr;
	}

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

	// 頂点の作成
	hr = MakeVertex(pDevice);
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
	if (FAILED(hr)) {
		return hr;
	}

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
	if (FAILED(hr)) {
		return hr;
	}

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
	if (FAILED(hr)) {
		return hr;
	}


	// 深度ステンシルテクスチャ
	D3D11_TEXTURE2D_DESC shadowMapDesc;
	ZeroMemory(&shadowMapDesc, sizeof(D3D11_TEXTURE2D_DESC));
	shadowMapDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	shadowMapDesc.MipLevels = 1;
	shadowMapDesc.ArraySize = 1;
	shadowMapDesc.SampleDesc.Count = 1;
	shadowMapDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	shadowMapDesc.Height = static_cast<UINT>(SCREEN_HEIGHT);
	shadowMapDesc.Width = static_cast<UINT>(SCREEN_WIDTH);

	hr = pDevice->CreateTexture2D(
		&shadowMapDesc,
		nullptr,
		&m_pDepthStencTexure
	);
	if (FAILED(hr)) {
		return hr;
	}

	// 深度ステンシルビュー
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	hr = pDevice->CreateDepthStencilView(
		m_pDepthStencTexure,
		&depthStencilViewDesc,
		&m_pDepthStencilView
	);
	if (FAILED(hr)) {
		return hr;
	}

	// 深度ステンシルシェーダーリソース
	D3D11_SHADER_RESOURCE_VIEW_DESC DSshaderResourceViewDesc;
	ZeroMemory(&DSshaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	DSshaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	DSshaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	DSshaderResourceViewDesc.Texture2D.MipLevels = 1;

	hr = pDevice->CreateShaderResourceView(
		m_pDepthStencTexure,
		&DSshaderResourceViewDesc,
		&m_pDepthStencResourceView
	);
	if (FAILED(hr)) {
		return hr;
	}

	// 深度ステンシルサンプルラー
	D3D11_SAMPLER_DESC comparisonSamplerDesc;
	ZeroMemory(&comparisonSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	comparisonSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	comparisonSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	comparisonSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	comparisonSamplerDesc.BorderColor[0] = 1.0f;
	comparisonSamplerDesc.BorderColor[1] = 1.0f;
	comparisonSamplerDesc.BorderColor[2] = 1.0f;
	comparisonSamplerDesc.BorderColor[3] = 1.0f;
	comparisonSamplerDesc.MinLOD = 0.f;
	comparisonSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	comparisonSamplerDesc.MipLODBias = 0.f;
	comparisonSamplerDesc.MaxAnisotropy = 0;
	comparisonSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	comparisonSamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;

	hr = pDevice->CreateSamplerState(
		&comparisonSamplerDesc,
		&m_pDepthSamplerState
	);
	if (FAILED(hr)) {
		return hr;
	}


	return hr;
}

//===================================
//
//	終了処理
//
//===================================
void PostProcessing::Uninit()
{


}

//===================================
//
//	描画処理
//
//===================================
void PostProcessing::Draw(ID3D11DeviceContext* pDeviceContext)
{
	// 拡縮
	XMMATRIX mWorld = XMMatrixScaling(m_siz.x, m_siz.y, m_siz.z);
	// 回転
	mWorld *= XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_rot.x),
		XMConvertToRadians(m_rot.y), XMConvertToRadians(m_rot.z));
	// 移動
	mWorld *= XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
	// ワールド マトリックスに設定
	XMStoreFloat4x4(&m_mWorld, mWorld);

	if (m_pRenderTexture) {
		// 拡縮
		mWorld = XMMatrixScaling(m_sizTex.x, m_sizTex.y, 1.0f);
		// 移動
		mWorld *= XMMatrixTranslation(m_posTex.x, m_posTex.y, 0.0f);
		// テクスチャ マトリックスに設定
		XMStoreFloat4x4(&m_mTex, mWorld);
	}
	else {
		// テクスチャ無し
		m_mTex._44 = 0.0f;
	}

	pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
	pDeviceContext->IASetInputLayout(m_pInputLayout);

	UINT stride = sizeof(VERTEX_2D);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	//===== サンプラー、テクスチャ =====
	ID3D11SamplerState* pState[2] = { m_pSamplerState, m_pDepthSamplerState };
	ID3D11ShaderResourceView* pResource[2] = { m_pTexureResourceView, m_pDepthStencResourceView};

	pDeviceContext->PSSetSamplers(0, 2, pState);
	pDeviceContext->PSSetShaderResources(0, 2, pResource);

	//pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerState);
	//pDeviceContext->PSSetShaderResources(0, 1, &m_pTexureResourceView);


	VERTEX_SHADER_GLOBAL vcb;
	vcb.mProj = XMMatrixTranspose(XMLoadFloat4x4(&m_mProj));
	vcb.mView = XMMatrixTranspose(XMLoadFloat4x4(&m_mView));
	vcb.mWorld = XMMatrixTranspose(XMLoadFloat4x4(&m_mWorld));
	vcb.mTex = XMMatrixTranspose(XMLoadFloat4x4(&m_mTex));
	pDeviceContext->UpdateSubresource(m_pVertexConstantBuffer, 0, nullptr, &vcb, 0, 0);
	pDeviceContext->VSSetConstantBuffers(0, 1, &m_pVertexConstantBuffer);

	PIXEL_SHADER_GLOBAL pcb;
	pcb.mTex = XMMatrixTranspose(XMLoadFloat4x4(&m_mTex));
	pcb.vColor = XMLoadFloat4(&m_col);
	// ガウスブラー
	pcb.nSampleCount = g_gaussParam.nSampleCount;
	for (int i = 0; i < 16; i++)
	{
		pcb.vOffset[i] = XMLoadFloat4(&g_gaussParam.Offset[i]);
	}
	pDeviceContext->UpdateSubresource(m_pPixelConstantBuffer, 0, nullptr, &pcb, 0, 0);
	pDeviceContext->PSSetConstantBuffers(1, 1, &m_pPixelConstantBuffer);

	// プリミティブ形状をセット
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// ポリゴンの描画
	pDeviceContext->Draw(NUM_VERTEX, 0);
}

HRESULT PostProcessing::MakeVertex(ID3D11Device* pDevice)
{
	// 頂点座標の設定
	m_vertexWk[0].vtx = XMFLOAT3(-0.5f, 0.5f, 0.0f);
	m_vertexWk[1].vtx = XMFLOAT3(0.5f, 0.5f, 0.0f);
	m_vertexWk[2].vtx = XMFLOAT3(-0.5f, -0.5f, 0.0f);
	m_vertexWk[3].vtx = XMFLOAT3(0.5f, -0.5f, 0.0f);
	
	// 拡散反射光の設定
	m_vertexWk[0].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_vertexWk[1].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_vertexWk[2].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_vertexWk[3].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	m_vertexWk[0].tex = XMFLOAT2(0.0f, 0.0f);
	m_vertexWk[1].tex = XMFLOAT2(1.0f, 0.0f);
	m_vertexWk[2].tex = XMFLOAT2(0.0f, 1.0f);
	m_vertexWk[3].tex = XMFLOAT2(1.0f, 1.0f);

	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(m_vertexWk);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = &m_vertexWk[0];

	HRESULT hr = pDevice->CreateBuffer(&vbd, &initData, &m_pVertexBuffer);

	return hr;
}



//-------------------------------------------------------------------------------------------------
//      ガウスの重みを計算します.
//-------------------------------------------------------------------------------------------------
float GaussianDistribution(const XMFLOAT2& pos, float rho)
{
	return exp(-(pos.x * pos.x + pos.y * pos.y) / (2.0f * rho * rho));
}

//-------------------------------------------------------------------------------------------------
//      ブラーパラメータを計算します.
//-------------------------------------------------------------------------------------------------
GaussBlurParam CalcBlurParam(int width, int height, XMFLOAT2 dir, float deviation)
{
	GaussBlurParam result;
	result.nSampleCount = 15;
	auto tu = 1.0f / float(width);
	auto tv = 1.0f / float(height);

	result.Offset[0].z = GaussianDistribution(XMFLOAT2(0.0f, 0.0f), deviation);
	auto total_weight = result.Offset[0].z;

	result.Offset[0].x = 0.0f;
	result.Offset[0].y = 0.0f;

	for (auto i = 1; i < 8; ++i)
	{
		result.Offset[i].x = dir.x * i * tu;
		result.Offset[i].y = dir.y * i * tv;
		result.Offset[i].z = GaussianDistribution(XMFLOAT2(dir.x * float(i), dir.y * float(i)), deviation);
		total_weight += result.Offset[i].z * 2.0f;
	}

	for (auto i = 0; i < 8; ++i)
	{
		result.Offset[i].z /= total_weight;
	}
	for (auto i = 8; i < 15; ++i)
	{
		result.Offset[i].x = -result.Offset[i - 7].x;
		result.Offset[i].y = -result.Offset[i - 7].y;
		result.Offset[i].z = result.Offset[i - 7].z;
	}

	return result;
}