//=============================================================================
//
// インスタンシングメッシュ共通処理 [instancingMesh.h]
// Author : KOSHI USAMI
//
//=============================================================================
#include "instancingMesh.h"
#include "../System/Shader.h"
#include "../System/Texture.h"

#include "../System/polygon.h"
#include "../System/input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define M_DIFFUSE		XMFLOAT4(1.0f,1.0f,1.0f,1.0f)
#define M_SPECULAR		XMFLOAT4(0.1f,0.1f,0.1f,1.0f)
#define M_AMBIENT		XMFLOAT4(1.0f,1.0f,1.0f,1.0f)
#define M_EMISSIVE		XMFLOAT4(0.0f,0.0f,0.0f,1.0f)
#define MAX_INSTANCE (1024)


//*****************************************************************************
// 構造体定義
//*****************************************************************************
// インスタンシングデータ
struct InstancingMatrix
{
	XMMATRIX mWorld;
	//XMMATRIX mTexture;
};

// シェーダに渡す値
struct SHADER_GLOBAL {
	XMMATRIX	mVP;		// ワールド×ビュー×射影行列(転置行列)
	XMMATRIX	mLightVP;		// ワールド×ビュー×射影行列(転置行列)
	XMMATRIX	mTexture;
	XMVECTOR	vFog;
};
struct INSTANCING_GLOBAL
{
	InstancingMatrix aInstancing[MAX_INSTANCE];
};

// インスタンシングマテリアル
//struct InstancingMaterial
//{
//	// マテリアル
//	XMVECTOR	vAmbient;	// アンビエント色(+テクスチャ有無)
//	XMVECTOR	vDiffuse;	// ディフューズ色
//	XMVECTOR	vSpecular;	// スペキュラ色(+スペキュラ強度)
//	XMVECTOR	vEmissive;	// エミッシブ色
//
//	int		bBump = false;
//};

struct SHADER_GLOBAL2 {
	XMVECTOR	vEye;		// 視点座標
	// 光源
	XMVECTOR	vLightDir;	// 光源方向
	XMVECTOR	vLa;		// 光源色(アンビエント)
	XMVECTOR	vLd;		// 光源色(ディフューズ)
	XMVECTOR	vLs;		// 光源色(スペキュラ)

		// マテリアル
	XMVECTOR	vAmbient;	// アンビエント色(+テクスチャ有無)
	XMVECTOR	vDiffuse;	// ディフューズ色
	XMVECTOR	vSpecular;	// スペキュラ色(+スペキュラ強度)
	XMVECTOR	vEmissive;	// エミッシブ色

	int		bBump = false;
};
//struct INSTANCING_GLOBAL2
//{
//	InstancingMaterial aInstancing[MAX_INSTANCE];
//};


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer*				g_pConstantBuffer[4];	// 定数バッファ
static ID3D11SamplerState*			g_pSamplerState;		// テクスチャ サンプラ
static ID3D11VertexShader*			g_pVertexShader;		// 頂点シェーダ
static ID3D11InputLayout*			g_pInputLayout;			// 頂点フォーマット
static ID3D11PixelShader*			g_pPixelShader;			// ピクセルシェーダ

static MATERIAL						g_material;				// マテリアル
static CCamera*						g_pCamera;				// カメラ
static CLight*						g_pLight;				// ライト

static const XMMATRIX SHADOW_BIAS = XMMATRIX(
	0.5f, 0.0f, 0.0f, 0.0f,
	0.0f, -0.5f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.5f, 0.5f, 0.0f, 1.0f);

static INSTANCING_GLOBAL icb;
//static INSTANCING_GLOBAL2 icb2;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitInstancingMesh(void)
{
	ID3D11Device* pDevice = GetDevice();
	HRESULT hr;

	// シェーダ初期化
	static const D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	hr = LoadShader("InstancingVertex", "InstancingPixel", &g_pVertexShader, &g_pInputLayout, &g_pPixelShader, layout, _countof(layout));
	if (FAILED(hr)) {
		return hr;
	}

	// 定数バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SHADER_GLOBAL);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = pDevice->CreateBuffer(&bd, nullptr, &g_pConstantBuffer[0]);
	if (FAILED(hr)) return hr;
	bd.ByteWidth = sizeof(SHADER_GLOBAL2);
	hr = pDevice->CreateBuffer(&bd, nullptr, &g_pConstantBuffer[1]);
	if (FAILED(hr)) return hr;
	bd.ByteWidth = sizeof(INSTANCING_GLOBAL);
	hr = pDevice->CreateBuffer(&bd, nullptr, &g_pConstantBuffer[2]);
	if (FAILED(hr)) return hr;
	/*bd.ByteWidth = sizeof(INSTANCING_GLOBAL2);
	hr = pDevice->CreateBuffer(&bd, nullptr, &g_pConstantBuffer[3]);
	if (FAILED(hr)) return hr;*/

	// テクスチャ サンプラ生成
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	hr = pDevice->CreateSamplerState(&sd, &g_pSamplerState);
	if (FAILED(hr)) {
		return hr;
	}

	// マテリアルの初期設定
	g_material.Diffuse = M_DIFFUSE;
	g_material.Ambient = M_AMBIENT;
	g_material.Specular = M_SPECULAR;
	g_material.Power = 0.0f;
	g_material.Emissive = M_EMISSIVE;

	return hr;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitInstancingMesh(void)
{
	// テクスチャ サンプラの開放
	SAFE_RELEASE(g_pSamplerState);
	// 定数バッファの解放
	for (int i = 0; i < _countof(g_pConstantBuffer); ++i) {
		SAFE_RELEASE(g_pConstantBuffer[i]);
	}
	// ピクセルシェーダ解放
	SAFE_RELEASE(g_pPixelShader);
	// 頂点フォーマット解放
	SAFE_RELEASE(g_pInputLayout);
	// 頂点シェーダ解放
	SAFE_RELEASE(g_pVertexShader);
}

////=============================================================================
//// 更新処理
////=============================================================================
//void UpdateInstancingMesh(InstancingMesh* pInstancingMesh)
//{
//	XMMATRIX mtxWorld, mtxRot, mtxTranslate;
//
//	if (!pInstancingMesh) return;
//
//	// ワールドマトリックスの初期化
//	mtxWorld = XMMatrixIdentity();
//
//	// 回転を反映
//	mtxRot = XMMatrixRotationRollPitchYaw(pInstancingMesh->rot.x, pInstancingMesh->rot.y, pInstancingMesh->rot.z);
//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);
//
//	// 移動を反映
//	mtxTranslate = XMMatrixTranslation(pInstancingMesh->pos.x, pInstancingMesh->pos.y, pInstancingMesh->pos.z);
//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);
//
//	// ワールドマトリックスの設定
//	XMStoreFloat4x4(&pInstancingMesh->mtxWorld, mtxWorld);
//}


void DrawInstancingMeshShadow(ID3D11DeviceContext* pDeviceContext, InstancingMesh* pInstancingMesh,
	std::vector<InstancingMeshData*>& InstancingList, int nTranslucntType)
{
	{
		// シャドウマップ
		// 各ターゲットビューをレンダーターゲットに設定
		pDeviceContext->OMSetRenderTargets(0, nullptr, g_pShadowDepthStencilView);

		// シェーダ設定
		pDeviceContext->VSSetShader(g_pVertexShader, nullptr, 0);
		pDeviceContext->PSSetShader(nullptr, nullptr, 0);
		pDeviceContext->IASetInputLayout(g_pInputLayout);

		// 頂点バッファをセット
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		pDeviceContext->IASetVertexBuffers(0, 1, &pInstancingMesh->pVertexBuffer, &stride, &offset);
		// インデックスバッファをセット
		pDeviceContext->IASetIndexBuffer(pInstancingMesh->pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		//===== サンプラー、テクスチャ =====
		ID3D11SamplerState* pState[2] = { g_pSamplerState, g_pShadowSamplerState };
		ID3D11ShaderResourceView* pResource[3] = { pInstancingMesh->pTexture, g_pShadowResourceView, pInstancingMesh->pNormalTexture };
		pDeviceContext->PSSetSamplers(0, 2, pState);
		pDeviceContext->PSSetShaderResources(0, 3, pResource);
		// プリミティブ
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		// 共通シェーダーデータ
		SHADER_GLOBAL cb;
		cb.mVP = XMMatrixTranspose(XMLoadFloat4x4(&g_pLight->GetViewMatrix()) * XMLoadFloat4x4(&g_pCamera->GetProjMatrix()));
		cb.mLightVP = XMMatrixTranspose(XMLoadFloat4x4(&g_pLight->GetViewMatrix()) * XMLoadFloat4x4(&g_pLight->GetProjMatrix()));
		cb.mTexture = XMMatrixTranspose(XMLoadFloat4x4(pInstancingMesh->mtxTexture));
		pDeviceContext->UpdateSubresource(g_pConstantBuffer[0], 0, nullptr, &cb, 0, 0);
		pDeviceContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer[0]);

		//================== インスタンシングデータ ==================
		// カメラ行列取得
		XMFLOAT4X4& mtxView = CCamera::GetMainCamera()->GetViewMatrix();

		int count = (int)InstancingList.size();
		int n = (count - 1) / MAX_INSTANCE + 1;
		for (int i = 0; i < n; i++) 
		{
			int m = (i < n - 1 ? MAX_INSTANCE : count - i * MAX_INSTANCE);
			for (int j = 0; j < m; ++j)
			{
				InstancingMeshData* pData = InstancingList[i * MAX_INSTANCE + j];

				if (pInstancingMesh->bBillboard)
				{
					// マトリックス作成
					XMMATRIX mtxWorld, mtxScale, mtxTranslate;
					// ワールドマトリックスの初期化
					mtxWorld = XMMatrixIdentity();
					XMFLOAT4X4* m_mtxWorld = pData->mtxWorld;
					XMStoreFloat4x4(m_mtxWorld, mtxWorld);

					m_mtxWorld->_11 = mtxView._11;
					m_mtxWorld->_12 = mtxView._21;
					m_mtxWorld->_13 = mtxView._31;
					m_mtxWorld->_21 = mtxView._12;
					m_mtxWorld->_22 = mtxView._22;
					m_mtxWorld->_23 = mtxView._32;
					m_mtxWorld->_31 = mtxView._13;
					m_mtxWorld->_32 = mtxView._23;
					m_mtxWorld->_33 = mtxView._33;

					mtxWorld = XMLoadFloat4x4(m_mtxWorld);

					// スケールを反映
					mtxScale = XMMatrixScaling(pData->pScale->x, pData->pScale->y, 1.0f);
					mtxWorld = XMMatrixMultiply(mtxScale, mtxWorld);

					// 移動を反映
					mtxTranslate = XMMatrixTranslation(pData->pPos->x, pData->pPos->y, pData->pPos->z);
					mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

					// ワールドマトリックスの設定
					XMStoreFloat4x4(m_mtxWorld, mtxWorld);
					icb.aInstancing[j].mWorld = XMMatrixTranspose(XMLoadFloat4x4(m_mtxWorld));
				}
				else
				{
					icb.aInstancing[j].mWorld = XMMatrixTranspose(XMLoadFloat4x4(pData->mtxWorld));
					//icb.aInstancing[j].mTexture = XMLoadFloat4x4(pData->mtxTexture);
				}
			}
			pDeviceContext->UpdateSubresource(g_pConstantBuffer[2], 0, nullptr, &icb, 0, 0);
			pDeviceContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer[2]);
			// ポリゴンの描画
			pDeviceContext->DrawIndexedInstanced(pInstancingMesh->nNumIndex, m, 0, 0, 0);
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawInstancingMesh(ID3D11DeviceContext* pDeviceContext, InstancingMesh* pInstancingMesh,
	std::vector<InstancingMeshData*>& InstancingList, int nTranslucntType)
{
	//=============================================================================================
	// 各ターゲットビューをレンダーターゲットに設定
	SetRenderTarget();

	// シェーダ設定
	pDeviceContext->VSSetShader(g_pVertexShader, nullptr, 0);
	pDeviceContext->PSSetShader(g_pPixelShader, nullptr, 0);
	pDeviceContext->IASetInputLayout(g_pInputLayout);

	// 頂点バッファをセット
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &pInstancingMesh->pVertexBuffer, &stride, &offset);
	// インデックスバッファをセット
	pDeviceContext->IASetIndexBuffer(pInstancingMesh->pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);


	//===== サンプラー、テクスチャ =====
	ID3D11SamplerState* pState[2] = { g_pSamplerState, g_pShadowSamplerState };
	ID3D11ShaderResourceView* pResource[3] = { pInstancingMesh->pTexture, g_pShadowResourceView, pInstancingMesh->pNormalTexture };

	pDeviceContext->PSSetSamplers(0, 2, pState);
	pDeviceContext->PSSetShaderResources(0, 3, pResource);

	SHADER_GLOBAL cb;
	cb.mVP = XMMatrixTranspose(XMLoadFloat4x4(&g_pCamera->GetViewMatrix()) * XMLoadFloat4x4(&g_pCamera->GetProjMatrix()));
	cb.mLightVP = XMMatrixTranspose(XMLoadFloat4x4(&g_pLight->GetViewMatrix()) * XMLoadFloat4x4(&g_pCamera->GetProjMatrix()) * SHADOW_BIAS);
	cb.mTexture = XMMatrixTranspose(XMLoadFloat4x4(pInstancingMesh->mtxTexture));
	XMFLOAT2 fog = { FOG_FAR_Z / (FOG_FAR_Z - FOG_NEAR_Z), -1 / (FOG_FAR_Z - FOG_NEAR_Z) };
	cb.vFog = XMLoadFloat2(&fog);
	pDeviceContext->UpdateSubresource(g_pConstantBuffer[0], 0, nullptr, &cb, 0, 0);
	pDeviceContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer[0]);

	SHADER_GLOBAL2 cb2;
	cb2.vEye = XMLoadFloat3(&g_pCamera->GetPos());
	cb2.vLightDir = XMLoadFloat3(&g_pLight->GetDir());
	cb2.vLa = XMLoadFloat4(&g_pLight->GetAmbient());
	cb2.vLd = XMLoadFloat4(&g_pLight->GetDiffuse());
	cb2.vLs = XMLoadFloat4(&g_pLight->GetSpecular());

	cb2.vDiffuse = XMLoadFloat4(&pInstancingMesh->material.Diffuse);
	cb2.vAmbient = XMVectorSet(pInstancingMesh->material.Ambient.x, pInstancingMesh->material.Ambient.y, pInstancingMesh->material.Ambient.z, (pInstancingMesh->pTexture != nullptr) ? 1.f : 0.f);
	cb2.vSpecular = XMVectorSet(pInstancingMesh->material.Specular.x, pInstancingMesh->material.Specular.y, pInstancingMesh->material.Specular.z, pInstancingMesh->material.Power);
	cb2.vEmissive = XMLoadFloat4(&pInstancingMesh->material.Emissive);

	cb2.bBump = pInstancingMesh->bBump;

	pDeviceContext->UpdateSubresource(g_pConstantBuffer[1], 0, nullptr, &cb2, 0, 0);
	pDeviceContext->PSSetConstantBuffers(1, 1, &g_pConstantBuffer[1]);

	// プリミティブ形状をセット
	static const D3D11_PRIMITIVE_TOPOLOGY pt[] = {
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,	// 0なら三角形ストリップ
		D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
		D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
	};
	pDeviceContext->IASetPrimitiveTopology(pt[pInstancingMesh->primitiveType]);

	// ブレンドステート
	SetBlendState(pInstancingMesh->blendState);

	//================== インスタンシングデータ ==================
			// カメラ行列取得
	XMFLOAT4X4& mtxView = CCamera::GetMainCamera()->GetViewMatrix();

	int count = (int)InstancingList.size();
	int n = (count - 1) / MAX_INSTANCE + 1;
	for (int i = 0; i < n; i++)
	{
		int m = (i < n - 1 ? MAX_INSTANCE : count - i * MAX_INSTANCE);
		for (int j = 0; j < m; ++j)
		{
			InstancingMeshData* pData = InstancingList[i * MAX_INSTANCE + j];

			if (pInstancingMesh->bBillboard)
			{
				// マトリックス作成
				XMMATRIX mtxWorld, mtxScale, mtxTranslate;
				// ワールドマトリックスの初期化
				mtxWorld = XMMatrixIdentity();
				XMFLOAT4X4* m_mtxWorld = pData->mtxWorld;
				XMStoreFloat4x4(m_mtxWorld, mtxWorld);

				m_mtxWorld->_11 = mtxView._11;
				m_mtxWorld->_12 = mtxView._21;
				m_mtxWorld->_13 = mtxView._31;
				m_mtxWorld->_21 = mtxView._12;
				m_mtxWorld->_22 = mtxView._22;
				m_mtxWorld->_23 = mtxView._32;
				m_mtxWorld->_31 = mtxView._13;
				m_mtxWorld->_32 = mtxView._23;
				m_mtxWorld->_33 = mtxView._33;

				mtxWorld = XMLoadFloat4x4(m_mtxWorld);

				// スケールを反映
				mtxScale = XMMatrixScaling(pData->pScale->x, pData->pScale->y, 1.0f);
				mtxWorld = XMMatrixMultiply(mtxScale, mtxWorld);

				// 移動を反映
				mtxTranslate = XMMatrixTranslation(pData->pPos->x, pData->pPos->y, pData->pPos->z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

				// ワールドマトリックスの設定
				XMStoreFloat4x4(m_mtxWorld, mtxWorld);
				icb.aInstancing[j].mWorld = XMMatrixTranspose(XMLoadFloat4x4(m_mtxWorld));
			}
			else
			{
				icb.aInstancing[j].mWorld = XMMatrixTranspose(XMLoadFloat4x4(pData->mtxWorld));
				//icb.aInstancing[j].mTexture = XMLoadFloat4x4(pData->mtxTexture);
			}
		}
		pDeviceContext->UpdateSubresource(g_pConstantBuffer[2], 0, nullptr, &icb, 0, 0);
		pDeviceContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer[2]);
		/*pDeviceContext->UpdateSubresource(g_pConstantBuffer[3], 0, nullptr, &icb2, 0, 0);
		pDeviceContext->PSSetConstantBuffers(3, 1, &g_pConstantBuffer[3]);*/

		// ポリゴンの描画
		pDeviceContext->DrawIndexedInstanced(pInstancingMesh->nNumIndex, m, 0, 0, 0);
	}

	// ブレンドステート
	SetBlendState(BS_NONE);
}

//=============================================================================
// 頂点の作成
//=============================================================================
HRESULT MakeInstancingMeshVertex(ID3D11Device* pDevice, InstancingMesh* pInstancingMesh, VERTEX_3D vertexWk[], int indexWk[])
{
	pInstancingMesh->fAlpha = 1.0f;
	for (int i = 0; i < pInstancingMesh->nNumVertex; ++i) {
		if (pInstancingMesh->fAlpha > vertexWk[i].diffuse.w) {
			pInstancingMesh->fAlpha = vertexWk[i].diffuse.w;
		}
	}

	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(VERTEX_3D) * pInstancingMesh->nNumVertex;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = vertexWk;
	HRESULT hr = pDevice->CreateBuffer(&vbd, &initData, &pInstancingMesh->pVertexBuffer);
	if (FAILED(hr)) {
		return hr;
	}

	CD3D11_BUFFER_DESC ibd(pInstancingMesh->nNumIndex * sizeof(int), D3D11_BIND_INDEX_BUFFER);
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = indexWk;
	hr = pDevice->CreateBuffer(&ibd, &initData, &pInstancingMesh->pIndexBuffer);

	return hr;
}

//=============================================================================
// メッシュ解放
//=============================================================================
void ReleaseInstancingMesh(InstancingMesh* pInstancingMesh)
{
	if (!pInstancingMesh) return;
	// テクスチャ解放
	SAFE_RELEASE(pInstancingMesh->pTexture);
	// 頂点バッファ解放
	SAFE_RELEASE(pInstancingMesh->pVertexBuffer);
	// インデックス バッファ解放
	SAFE_RELEASE(pInstancingMesh->pIndexBuffer);
}

//=============================================================================
// カメラの設定
//=============================================================================
void SetInstancingMeshCamera(CCamera* pCamera)
{
	g_pCamera = pCamera;
}

//=============================================================================
// ライトの設定
//=============================================================================
void SetInstancingMeshLight(CLight* pLight)
{
	g_pLight = pLight;
}

//=============================================================================
// 頂点バッファ更新
//=============================================================================
HRESULT UpdateInstancingMeshVertex(ID3D11DeviceContext* pDeviceContext, InstancingMesh* pInstancingMesh, VERTEX_3D vertex[])
{
	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	D3D11_MAPPED_SUBRESOURCE msr;
	HRESULT hr = pDeviceContext->Map(pInstancingMesh->pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	if (SUCCEEDED(hr)) {
		// 頂点データを上書き
		rsize_t len = sizeof(VERTEX_3D) * pInstancingMesh->nNumVertex;
		memcpy_s(msr.pData, len, vertex, len);
		// 頂点データをアンロックする
		pDeviceContext->Unmap(pInstancingMesh->pVertexBuffer, 0);
	}
	return hr;
}
