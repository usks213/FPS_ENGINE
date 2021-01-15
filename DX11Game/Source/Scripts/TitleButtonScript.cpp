//==================================================================
//								TitleButtonScript.cpp
//	タイトルボタンレイアウト
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/15	タイトルボタンクラス作成
//
//===================================================================


//====== インクルード部 ======

// 自身
#include "TitleButtonScript.h"

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
#include "../Engine/ECSWorld/GameWorld.h"

// ネームスペース
using namespace ECS;


//===== マクロ定義 =====



//******************** スクリプトメソッド ********************


//========================================
//
//	開始時
//
//========================================
void TitleButtonScript::Start()
{
	// サイズ
	transform().lock()->m_scale = Vector3{ 1920 / 4, 320 / 4, 1 };

	// スプライト
	const auto& spr = gameObject().lock()->AddComponent<SpriteRenderer>();
	spr->SetRendererBlendState(BS_ALPHABLEND);
	spr->SetDiffuseTexture("data/texture/TitleButton.png");
	spr->SetLayer(spr->eUI1);
	spr->SetAlpha(1);
	
	m_sprite = spr;
}

//========================================
//
//	更新時
//
//========================================
void TitleButtonScript::Update()
{
	// シーン遷移
	if (GetMouseRelease(MOUSEBUTTON_L) || GetMouseRelease(MOUSEBUTTON_R) ||
		GetKeyRelease(VK_RETURN) || GetKeyRelease(VK_SPACE))
	{
		// ゲームスタート
		WorldManager::GetInstance()->LoadWorld<GameWorld>("Game");
	}
}

//========================================
//
//	後更新時
//
//========================================
void TitleButtonScript::LateUpdate()
{
	// 点滅処理
	m_nCount++;

	if (m_nCount % 90 < 60)
	{
		// 表示
		m_sprite.lock()->SetAlpha(1);
	}
	else
	{
		// 非表示
		m_sprite.lock()->SetAlpha(0);
	}
}

//========================================
//
//	終了時
//
//========================================
void TitleButtonScript::End()
{
}

