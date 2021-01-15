//==================================================================
//								BillboardRenderer.cpp
//	ビルボードレンダラー
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/02	ECS用に再設計
//
//===================================================================


//====== インクルード部 ======
#include "BillboardRenderer.h"
#include "../ECSSystem/RendererSystem.h"
#include "../ECS/Entity/EntityManager.h"

#include "Transform.h"
#include "../Renderer/Camera.h"
#include "../System/Texture.h"

using namespace ECS;

//===== マクロ定義 =====

//===== グローバル変数 =====
MESH BillboardRenderer::m_masterMesh;


//========================================
//
//	コンストラクタ
//
//========================================
BillboardRenderer::BillboardRenderer()
{
	m_material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_material.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_material.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_material.Emissive = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_material.Power = 0.0f;

	m_diffuseTexture = nullptr;
	m_eTranslucentType = eTranslucentType::TT_NOAFFECT;

	// メッシュの取得
	m_mesh = GetBillboardMesh();
}

//========================================
//
//	デストラクタ
//
//========================================
BillboardRenderer::~BillboardRenderer()
{
}

//========================================
//
//	前描画
//
//========================================
void BillboardRenderer::EarlyDraw(ID3D11DeviceContext* pDC)
{
	const auto& mesh = m_mesh;

	if (mesh)
	{
		m_mesh->mtxTexture = m_mtxTexture;
		m_mesh->pMaterial = &m_material;
		m_mesh->pTexture = m_diffuseTexture;

		DrawMeshShadow(pDC, mesh);
	}
}

//========================================
//
//	後描画
//
//========================================
void BillboardRenderer::LateDraw(ID3D11DeviceContext* pDC)
{
	const auto& mesh = m_mesh;

	if (mesh)
	{
		// マトリックス作成
		XMMATRIX mtxWorld, mtxScale, mtxTranslate;
		// カメラ行列取得
		XMFLOAT4X4& mtxView = CCamera::GetMainCamera()->GetViewMatrix();
		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();
		XMFLOAT4X4* m_mtxWorld = m_transform.lock()->GetWorldMatrix();
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
		Vector3 scale = m_transform.lock()->m_scale;
		mtxScale = XMMatrixScaling(scale->x, scale->y, 1.0f);
		mtxWorld = XMMatrixMultiply(mtxScale, mtxWorld);

		// 移動を反映
		Vector3 pos = m_transform.lock()->m_pos;
		mtxTranslate = XMMatrixTranslation(pos->x, pos->y, pos->z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		XMStoreFloat4x4(m_mtxWorld, mtxWorld);

		m_mesh->mtxWorld = *m_mtxWorld;

		m_mesh->mtxTexture = m_mtxTexture;
		m_mesh->pMaterial = &m_material;
		m_mesh->pTexture = m_diffuseTexture;

		SetBlendState(m_eState);
		// Zバッファ無効
		SetZBuffer(false);

		DrawMesh(pDC, mesh, m_eTranslucentType);

		SetBlendState(BS_NONE);

		// Zバッファ
		SetZBuffer(true);
	}
}

//========================================
//
// メッシュの取得
//
//========================================
MESH* BillboardRenderer:: GetBillboardMesh()
{
	// メッシュが生成されている
	if (m_masterMesh.pVertexBuffer || m_masterMesh.pIndexBuffer ||
		m_masterMesh.nNumVertex || m_masterMesh.nNumIndex)
	{
		return &m_masterMesh;
	}

	// オブジェクトの頂点配列を生成
	m_masterMesh.nNumVertex = 4;
	VERTEX_3D* pVertexWk = new VERTEX_3D[m_masterMesh.nNumVertex];

	// 頂点配列の中身を埋める
	VERTEX_3D* pVtx = pVertexWk;

	// 頂点座標の設定
	pVtx[0].vtx = XMFLOAT3(-1.0f / 2, -1.0f / 2, 0.0f);
	pVtx[1].vtx = XMFLOAT3(-1.0f / 2, 1.0f / 2, 0.0f);
	pVtx[2].vtx = XMFLOAT3(1.0f / 2, -1.0f / 2, 0.0f);
	pVtx[3].vtx = XMFLOAT3(1.0f / 2, 1.0f / 2, 0.0f);

	// 法線の設定
	pVtx[0].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVtx[1].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVtx[2].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVtx[3].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// 反射光の設定
	pVtx[0].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	pVtx[0].tex = XMFLOAT2(0.0f, 1.0f);
	pVtx[1].tex = XMFLOAT2(0.0f, 0.0f);
	pVtx[2].tex = XMFLOAT2(1.0f, 1.0f);
	pVtx[3].tex = XMFLOAT2(1.0f, 0.0f);

	// インデックス配列を生成
	m_masterMesh.nNumIndex = 4;
	int* pIndexWk = new int[m_masterMesh.nNumIndex];

	// インデックス配列の中身を埋める
	pIndexWk[0] = 0;
	pIndexWk[1] = 1;
	pIndexWk[2] = 2;
	pIndexWk[3] = 3;

	ID3D11Device* pDevice = GetDevice();
	// 頂点バッファ生成
	HRESULT hr = MakeMeshVertex(pDevice, &m_masterMesh, pVertexWk, pIndexWk);

	// 一時配列の解放
	delete[] pIndexWk;
	delete[] pVertexWk;

	m_masterMesh.bLight = false;

	return &m_masterMesh;
}

//========================================
//
// ベースカラーテクスチャのセット
//
//========================================
void BillboardRenderer::SetDiffuseTexture(const char* filename)
{
	m_diffuseTexture = CTexture::GetTexture(GetDevice(), filename);
}

//========================================
//
// ベースカラーテクスチャのセット
//
//========================================
void BillboardRenderer::SetDiffuseTexture(const wchar_t* filename)
{
	m_diffuseTexture = CTexture::GetTexture(GetDevice(), filename);
}