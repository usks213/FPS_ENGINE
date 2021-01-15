//==================================================================
//								GameOverScript.h
//	ゲームオーバー演出
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/15	ゲームオーバークラス作成
//
//===================================================================


//====== インクルード部 ======

// 自身
#include "GameOverScript.h"

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

// ワールド
#include "../Engine/ECS/World/WorldManager.h"
#include "../Engine/ECSWorld/TitleWorld.h"


// ネームスペース
using namespace ECS;


//===== マクロ定義 =====



//******************** スクリプトメソッド ********************


//========================================
//
//	開始時
//
//========================================
void GameOverScript::Start()
{
	// スケール
	transform().lock()->m_scale = 
		Vector3{ SCREEN_WIDTH, SCREEN_HEIGHT, 0 };

	// スプライト
	const auto& Spr = gameObject().lock()->AddComponent<SpriteRenderer>();
	Spr->SetDiffuseTexture("data/texture/GameOver.png");
	Spr->SetLayer(Spr->eUI3);
	Spr->SetRendererBlendState(BS_ALPHABLEND);
	m_sprite = Spr;

	m_fAlpha = 0.0f;
	m_fAddAlpha = 1.0f / 60.0f;

	// サウンド
	CSound::StopBGM("GameBGM.mp3");
	CSound::PlaySE("GameOver.wav", 1.0f);

}

//========================================
//
//	更新時
//
//========================================
void GameOverScript::Update()
{
}

//========================================
//
//	後更新時
//
//========================================
void GameOverScript::LateUpdate()
{
	m_fAlpha += m_fAddAlpha;
	m_sprite.lock()->SetAlpha(m_fAlpha);

	if (m_fAlpha > 2.0f)
	{
		// タイトルへ
		// ゲームオーバー
		WorldManager::GetInstance()->LoadWorld<TitleWorld>("Title");
	}
}

//========================================
//
//	終了時
//
//========================================
void GameOverScript::End()
{
}

