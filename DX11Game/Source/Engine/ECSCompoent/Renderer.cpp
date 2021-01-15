//==================================================================
//								Renderer.h
//	レンダラー
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/23	ECS用に再設計
//
//===================================================================


//====== インクルード部 ======
#include "Renderer.h"
#include "../ECSSystem/RendererSystem.h"
#include "../ECS/Entity/EntityManager.h"

#include "Transform.h"
#include "../main.h"
#include "../Renderer/Camera.h"

using namespace ECS;

//===== マクロ定義 =====



//========================================
//
//	コンストラクタ
//
//========================================
Renderer::Renderer()
{
	// テクスチャ座標
	m_texPos = Vector3(0.0f, 0.0f, 0.0f);
	m_texSize = Vector3(1.0f, 1.0f, 1.0f);
	XMMATRIX mTex = XMMatrixScaling(m_texSize->x, m_texSize->y, 0.0f);
	mTex *= XMMatrixTranslation(m_texPos->x, m_texPos->y, 1.0f);
	XMStoreFloat4x4(&m_mtxTexture, mTex);

	// ブレンドステート
	m_eState = BS_NONE;

	m_bUpdateLayer = true;
}

//========================================
//
//	デストラクタ
//
//========================================
Renderer::~Renderer()
{

}

//========================================
//
//	生成時
//
//========================================
void Renderer::OnCreate()
{
	// システムに格納
	RendererSystem* sys = GetEntityManager()->GetWorld()->GetSystem<RendererSystem>();
	if (sys) sys->AddList(this);

	// トランスフォームから取得
	m_transform = m_Parent.lock()->GetComponent<Transform>();
}

//========================================
//
//	破棄時
//
//========================================
void Renderer::OnDestroy()
{
	// システムから除外
	RendererSystem* sys = GetEntityManager()->GetWorld()->GetSystem<RendererSystem>();
	if (sys) sys->RemoveList(this);
}


//========================================
//
//	カメラからの距離を計算
//
//========================================
void Renderer::LayerUpdate()
{
	if (!m_bUpdateLayer) return;

	// カメラ座標
	Vector3 cameraPos = CCamera::GetMainCamera()->GetPos();
	// 距離を格納
	m_fLayer = Vector3::Length(m_transform.lock()->m_pos - cameraPos);
}

//========================================
//
// テクスチャマトリックスの更新
//
//========================================
void Renderer::UpdateTexMatrix()
{
	// テクスチャマトリックス更新
	XMMATRIX mTex = XMMatrixScaling(m_texSize->x, m_texSize->y, 0.0f);
	mTex *= XMMatrixTranslation(m_texPos->x, m_texPos->y, 1.0f);
	XMStoreFloat4x4(&m_mtxTexture, mTex);
}