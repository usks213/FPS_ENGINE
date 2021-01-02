//==================================================================
//								AssimpRenderer.cpp
//	アシンプレンダラー
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
#include "AssimpRenderer.h"
#include "../ECSSystem/RendererSystem.h"
#include "../ECS/Entity/EntityManager.h"

#include "Transform.h"
#include "../System/Texture.h"

using namespace ECS;

//===== マクロ定義 =====


//===== グローバル変数 =====
std::map<const std::string, CAssimpModel*, std::less<>> AssimpRenderer::m_assimpModelPool;
std::map<const CAssimpModel*, TAssimpMaterial*, std::less<>> AssimpRenderer::m_materialPool;



//========================================
//
//	コンストラクタ
//
//========================================
AssimpRenderer::AssimpRenderer()
{
	m_model = nullptr;
	m_filename = "\0";
	m_diffuseColor = { 1,1,1,1 };
}

//========================================
//
//	デストラクタ
//
//========================================
AssimpRenderer::~AssimpRenderer()
{
}

//========================================
//
//	前描画
//
//========================================
void AssimpRenderer::EarlyDraw(ID3D11DeviceContext* pDC)
{
	const auto& model = m_model;

	if (model)
	{
		model->Draw(pDC, *m_transform.lock()->GetWorldMatrix(), eNoAffect, true);
	}
}

//========================================
//
//	後描画
//
//========================================
void AssimpRenderer::LateDraw(ID3D11DeviceContext* pDC)
{
	const auto& model = m_model;

	if (model)
	{
		// カラー変更
		TAssimpMaterial* pMat = m_model->GetMaterial();
		if (pMat)
		{
			pMat->Kd = m_diffuseColor;
		}

		// 描画
		bool bExitFlg = model->Draw(pDC, *m_transform.lock()->GetWorldMatrix(), eNoAffect, false);

		// ここでコールバック関数の処理？
		if (bExitFlg)
		{
			// コールバック関数
			m_Parent.lock()->SendComponentMessage("OnAnimationExit3D", this);
		}
	}
}


//========================================
//
// ベースカラーテクスチャのセット
//
//========================================
void AssimpRenderer::SetDiffuseTexture(const char* filename)
{
	// マテリアル取得
	auto pMat = GetMaterial();
	// テクスチャ
	if (pMat)
		pMat->pTexture = CTexture::GetTexture(GetDevice(), filename);
}

//========================================
//
// ベースカラーテクスチャのセット
//
//========================================
void AssimpRenderer::SetDiffuseTexture(const wchar_t* filename)
{
	// マテリアル取得
	auto pMat = GetMaterial();
	// テクスチャ
	if (pMat)
		pMat->pTexture = CTexture::GetTexture(GetDevice(), filename);
}

//========================================
//
// モデルロード
//
//========================================
bool AssimpRenderer::ModelLoad(std::string filename)
{
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pDC = GetDeviceContext();

	// プールから検索
	const auto& itr = m_assimpModelPool.find(filename);

	// 既にプールにあったら
	if (m_assimpModelPool.end() != itr)
	{
		m_model = itr->second;
		m_filename = filename;
		return true;
	}

	// なかったら新しく生成する
	const auto& am = new CAssimpModel();

	// モデルのロード
	bool b = am->Load(pDevice, pDC, filename);

	// プールに登録
	m_assimpModelPool.emplace(filename, am);

	// 格納
	m_model = am;
	m_filename = filename;

	// ライトとカメラ
	m_model->SetLight(CLight::GetMainLight());
	m_model->SetCamera(CCamera::GetMainCamera());

	return b;
}

//========================================
//
// モデル解放
//
//========================================
void AssimpRenderer::ModelRelese()
{
	const auto& model = m_model;

	if (model)
	{
		model->Release();
	}

	// プールから検索
	const auto& itr = m_assimpModelPool.find(m_filename);

	// プールにあったら
	if (m_assimpModelPool.end() != itr)
	{
		m_assimpModelPool.erase(itr);
	}

	if (m_model)
	{
		delete m_model;
		m_model = nullptr;
		m_filename = "\0";
	}
}

//========================================
//
// マテリアル取得
//
//========================================
TAssimpMaterial* AssimpRenderer::GetMaterial()
{
	// モデルがなかったら
	if (!m_model) return nullptr;

	// マテリアルがあったら
	if (m_model->GetMaterial()) return m_model->GetMaterial();

	// マテリアル検索
	const auto& itr = m_materialPool.find(m_model);

	// マテリアルがプールにあったら
	if (m_materialPool.end() != itr)
	{
		return itr->second;
	}

	// 新規作成
	auto pMat = new TAssimpMaterial();

	// モデルに格納
	m_model->SetMaterial(pMat);

	// プールに格納
	m_materialPool.emplace(m_model, pMat);

	return pMat;
}