//==================================================================
//								EnemyEffectScript.cpp
//	エネミーエフェクト
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/15	エネミーエフェクトクラス作成
//
//===================================================================


//====== インクルード部 ======

// 自身
#include "EnemyEffectScript.h"

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
#include "../Engine/ECSCompoent/Animator2D.h"
#include "../Engine/ECSCompoent/Animation2D.h"

// ECSコンポーネント
#include "../Engine/ECSCompoent/DeltaCollider.h"

// ECSシステム
#include "../Engine/ECSSystem/DeltaCollisionSystem.h"


// ネームスペース
using namespace ECS;


//===== マクロ定義 =====



//******************** スクリプトメソッド ********************


//========================================
//
//	開始時
//
//========================================
void EnemyEffectScript::Start()
{
	// スケール
	transform().lock()->m_scale = Vector3{ 400,400,400 };

	// ビルボードレンダラー
	const auto& bill = gameObject().lock()->AddComponent<BillboardRenderer>();
	bill->SetDiffuseTexture("data/texture/tktk_Darkness_5.png");
	bill->SetRendererBlendState(BS_ADDITIVE);

	// アニメーション
	const auto& anim = gameObject().lock()->AddComponent<Animator2D>();
	anim->SetSplit(5, 2);
	anim->AddAnimationDefault("Effect", 5 * 2);
	anim->SetAnimationSpeed(2);

	anim->SetAnimation("Effect");

	bill->UpdateTexMatrix();
}

//========================================
//
//	更新時
//
//========================================
void EnemyEffectScript::Update()
{
}

//========================================
//
//	後更新時
//
//========================================
void EnemyEffectScript::LateUpdate()
{
}

//========================================
//
//	終了時
//
//========================================
void EnemyEffectScript::End()
{
}

//========================================
//
//	アニメーション終了時コールバック関数
//
//========================================
void EnemyEffectScript::OnAnimationExit2D(Animator2D* animator)
{
	if (animator->GetCurrentAnimation().lock()->GetName() == "Effect")
	{
		// 自身の破棄
		Destroy(gameObject().lock());
	}
}