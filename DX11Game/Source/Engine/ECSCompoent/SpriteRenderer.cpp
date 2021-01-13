//==================================================================
//								SpriteRenderer.cpp
//	スプライトレンダラー
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
#include "SpriteRenderer.h"
#include "../ECSSystem/RendererSystem.h"
#include "../ECS/Entity/EntityManager.h"

#include "Transform.h"
#include "../Renderer/Camera.h"
#include "../System/Texture.h"
#include "../System/polygon.h"


using namespace ECS;


//===== マクロ定義 =====


//===== グローバル変数 =====


//========================================
//
//	コンストラクタ
//
//========================================
SpriteRenderer::SpriteRenderer()
{
	m_fLayer = eDefault;
	m_pTexture = nullptr;
	// 色
	m_color = Vector3{ 1,1,1 };
	m_fAlpha = 1.0f;

	// ブレンドステート
	m_eState = BS_ALPHABLEND;
}

//========================================
//
//	デストラクタ
//
//========================================
SpriteRenderer::~SpriteRenderer()
{
}

//========================================
//
//	前描画
//
//========================================
void SpriteRenderer::EarlyDraw(ID3D11DeviceContext* pDC)
{
}

//========================================
//
//	後描画
//
//========================================
void SpriteRenderer::LateDraw(ID3D11DeviceContext* pDC)
{
	// 判定
	const auto& trans = m_transform.lock();
	if (!trans) return;

	// Zバッファ無効
	SetZBuffer(false);
	// ブレンドステート
	SetBlendState(m_eState);

	// 座標
	SetPolygonPos(trans->m_pos->x, trans->m_pos->y);
	// 角度
	SetPolygonAngle(trans->m_rot->z);
	// サイズ
	SetPolygonSize(trans->m_scale->x, trans->m_scale->y);
	// カラー
	SetPolygonColor(m_color->x, m_color->y, m_color->z);
	// アルファ
	SetPolygonAlpha(m_fAlpha);
	// テクスチャ座標
	SetPolygonUV(m_texPos->x, m_texPos->y);
	// テクスチャサイズ
	SetPolygonFrameSize(m_texSize->x, m_texSize->y);
	// テクスチャ
	SetPolygonTexture(m_pTexture);

	// 描画
	DrawPolygon(pDC);

	// ブレンドステート
	SetBlendState(BS_NONE);
	// Zバッファ有効
	SetZBuffer(true);
}


//========================================
//
// ベースカラーテクスチャのセット
//
//========================================
void SpriteRenderer::SetDiffuseTexture(const char* filename)
{
	// テクスチャのセット
	m_pTexture = CTexture::GetTexture(GetDevice(), filename);
}

//========================================
//
// ベースカラーテクスチャのセット
//
//========================================
void SpriteRenderer::SetDiffuseTexture(const wchar_t* filename)
{
	// テクスチャのセット
	m_pTexture = CTexture::GetTexture(GetDevice(), filename);
}