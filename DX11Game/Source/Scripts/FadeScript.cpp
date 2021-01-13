//==================================================================
//								FadeScript.cpp
//	フェードスクリプト
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/13	フェードスクリプトクラス作成
//
//===================================================================


//====== インクルード部 ======

// 自身
#include "FadeScript.h"

// システム
#include "../Engine/System/input.h"
#include "../Engine/System/Sound.h"

// マネージャー
#include "../Engine/ECS/Entity/EntityManager.h"

// エンティティ
#include "../Engine/ECSEntity/GameObject.h"

// コンポーネント
#include "../Engine/ECSCompoent/Transform.h"
#include "../Engine/ECSCompoent/MeshRenderer.h"
#include "../Engine/ECSCompoent/BillboardRenderer.h"
#include "../Engine/ECSCompoent/AssimpRenderer.h"
#include "../Engine/ECSCompoent/SpriteRenderer.h"
#include "../Engine/ECSCompoent/InstancingMeshRenderer.h"
#include "../Engine/ECSCompoent/Rigidbody.h"
#include "../Engine/ECSCompoent/BoxCollider.h"
#include "../Engine/ECSCompoent/SphereCollider.h"

// ECSコンポーネント
#include "../Engine/ECSCompoent/DeltaCollider.h"

// ECSシステム
#include "../Engine/ECSSystem/DeltaCollisionSystem.h"


// ネームスペース
using namespace ECS;

//===== マクロ定義 =====


//===== 列挙 =====
namespace
{
	enum E_STATE_FADE	// フェードの状態の列挙
	{
		E_STATE_FADE_NODE = 0,
		E_STATE_FADE_OUT,
		E_STATE_FADE_IN,

		E_STATE_FADE_MAX,
	};
}



//******************** スクリプトメソッド ********************


//========================================
//
//	開始時
//
//========================================
void FadeScript::Start()
{
	// スケール
	m_nAlpha = 0;
	// ステート
	m_nState = E_STATE_FADE_NODE;
	// フレーム
	m_nFrame = 60;
	// 関数オブジェクト
	m_fadeEndFunc = nullptr;

	// ゲームオブジェクト
	gameObject().lock()->SetName("Fade");
	gameObject().lock()->SetTag("Fade");

	// レンダラー取得
	const auto& render = gameObject().lock()->AddComponent<SpriteRenderer>();
	render->SetDiffuseColor({1,1,1});
	render->SetAlpha(m_nAlpha);
	render->SetLayer(render->eFade1);
	render->SetRendererBlendState(EBlendState::BS_ALPHABLEND);
	// テクスチャ
	render->SetDiffuseTexture("data/texture/FadeDelta.png");
	m_render = render;

	// サイズ
	transform().lock()->m_scale = Vector3{ SCREEN_WIDTH, SCREEN_HEIGHT, 1 };

}

//========================================
//
//	更新時
//
//========================================
void FadeScript::Update()
{
}

//========================================
//
//	後更新時
//
//========================================
void FadeScript::LateUpdate()
{
	// 状態により分岐
	switch (m_nState)
	{
	case(E_STATE_FADE_IN):		// フェードイン
		// α値の減少
		m_nAlpha -= (int)(255.0f / m_nFrame);

		// α値が0より小さくなったら終了
		if (m_nAlpha < 0)
		{
			m_nAlpha = 0;
			m_nState = E_STATE_FADE_NODE;
			m_bNowFade = false;

			// フェードの消去
			Destroy(gameObject().lock());
		}

		// α値の更新
		m_render.lock()->SetAlpha(m_nAlpha / 255.0f);
		break;

	case(E_STATE_FADE_OUT):		// フェードアウト
		// α値の増加
		m_nAlpha += (int)(255.0f / m_nFrame);

		// α値が255を超えたらフェードインへ
		if (m_nAlpha > 255)
		{
			m_nAlpha = 255;
			m_nState = E_STATE_FADE_IN;

			// フェードアウトが終わったときに実装する処理
			// シーンの移動
			if (m_fadeEndFunc) m_fadeEndFunc();

			// サウンド

		}

		// α値の更新
		m_render.lock()->SetAlpha(m_nAlpha / 255.0f);
		break;

	default:
		break;
	}

	// α値の更新
	m_render.lock()->SetAlpha(m_nAlpha / 255.0f);
}

//========================================
//
//	終了時
//
//========================================
void FadeScript::End()
{
}


//========================================
//
// フェードアウトの開始
//
//========================================
void FadeScript::StartFadeOut(int nFrame)
{
	// 既にフェード中の時は処理しない
	if (E_STATE_FADE_NODE != m_nState)
	{
		return;
	}

	m_nAlpha = 0;
	m_nState = E_STATE_FADE_OUT;
	m_bNowFade = true;
}