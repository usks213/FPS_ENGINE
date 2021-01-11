//==================================================================
//								BombEffectScript.cpp
//	ボムエフェクト
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/10	ボムエフェクトスクリプトクラス作成
//
//===================================================================


//====== インクルード部 ======

// 自身
#include "BombEffectScript.h"

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

// スクリプト
#include "DropDeltaScript.h"


// ネームスペース
using namespace ECS;


//===== マクロ定義 =====



//******************** スクリプトメソッド ********************


//========================================
//
//	開始時
//
//========================================
void BombEffectScript::Start()
{
	// 名前・タグ
	gameObject().lock()->SetName("BombEffect");
	gameObject().lock()->SetTag("BombEffect");

	// 大きさ
	transform().lock()->m_scale = Vector3(20, 20, 20);
	transform().lock()->m_rot->x = 90;
	//--- コンポーネンの追加

	// インスタンシングレンダラー
	const auto& rn = gameObject().lock()->AddComponent<InstancingMeshRenderer>();
	rn->MakeOctahedron("BombEffect");
	//rn->SetBillboard(true);
	rn->SetDiffuseColor({ 1,0,1,1 });
	rn->SetBlendStateData(BS_ADDITIVE);

	// ECSリジッドボディ
	const auto& rb = gameObject().lock()->AddComponent<Rigidbody>();
	rb->SetDrag({ 0.0f,0.0f,0.0f });
	rb->SetGravityForce({ 0,0,0 });
	rb->SetStaticFriction(0);
	rb->SetDynamicFriction(0);
	rb->SetMass(1);
	rb->SetTorqueDrag({ 0,0,0 });

	// 生存時間
	m_nExitTime = 180;
}

//========================================
//
//	更新時
//
//========================================
void BombEffectScript::Update()
{
	m_nExitTime--;
	if (m_nExitTime < 0)
	{
		// 自身の削除
		GetEntityManager()->DestroyEntity(gameObject().lock());
	}
}

//========================================
//
//	後更新時
//
//========================================
void BombEffectScript::LateUpdate()
{
}

//========================================
//
//	終了時
//
//========================================
void BombEffectScript::End()
{
}
