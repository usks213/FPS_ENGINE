//==================================================================
//								DropDeltaScript.cpp
//	敵からドロップするアイテム
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/09	ドロップデルタスクリプトクラス作成
//
//===================================================================


//====== インクルード部 ======

// 自身
#include "DropDeltaScript.h"

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
#include "../Engine/ECSCompoent/ECSRigidbody.h"
#include "../Engine/ECSCompoent/ECSSphereCollider.h"

// ECSシステム
#include "../Engine/ECSSystem/ECSRigidbodySystem.h"
#include "../Engine/ECSSystem/ECSSphereCollisionSystem.h"


// ネームスペース
using namespace ECS;


//===== マクロ定義 =====



//******************** スクリプトメソッド ********************


//========================================
//
//	開始時
//
//========================================
void DropDeltaScript::Start()
{
	// 名前・タグ
	gameObject().lock()->SetName("DropDelta");
	gameObject().lock()->SetTag("DropDelta");

	// 大きさ
	transform().lock()->m_scale = Vector3(100, 100, 100);
	transform().lock()->m_rot->x = 90;

	//--- コンポーネンの追加

	// インスタンシングレンダラー
	gameObject().lock()->AddComponent<InstancingMeshRenderer>()->MakeTetraheron("DropDelta");

	// ECSリジッドボディ
	const auto& rb = gameObject().lock()->AddComponent<ECSRigidbody>();
	rb->GetData()->SetStaticFriction(0);
	rb->GetData()->SetDynamicFriction(0);
	rb->GetData()->SetMass(1);
	rb->GetData()->SetTorqueDrag({ 0,0,0 });
	// 回転
	rb->GetData()->AddTorqueY(rand() % 3 + 1);


	// ECSコライダー
	gameObject().lock()->AddComponent<ECSSphereCollider>()->GetData()->SetMain(false);


	// 生存時間
	m_nExitTime = 300;
}

//========================================
//
//	更新時
//
//========================================
void DropDeltaScript::Update()
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
void DropDeltaScript::LateUpdate()
{
}

//========================================
//
//	終了時
//
//========================================
void DropDeltaScript::End()
{
}


//******************** コールバック関数 ********************


//========================================
//
// 当たった時
//
//========================================
void DropDeltaScript::OnCollisionEnter(Collider* collider)
{

}

//========================================
//
// 当たっている間
//
//========================================
void DropDeltaScript::OnCollisionStay(Collider* collider)
{

}

//========================================
//
// 離れた時
//
//========================================
void DropDeltaScript::OnCollisionExit(Collider* collider)
{

}


//===== ECS =====

//========================================
//
// 当たった時
//
//========================================
void DropDeltaScript::OnECSCollisionEnter(SphereColliderData* collider)
{
	if (collider->gameObject().lock()->tag() == "Player")
	{
		// 自身の削除
		GetEntityManager()->DestroyEntity(gameObject().lock());
	}
}

//========================================
//
// 当たっている間
//
//========================================
void DropDeltaScript::OnECSCollisionStay(SphereColliderData* collider)
{
	if (collider->gameObject().lock()->tag() == "Player")
	{
		// 自身の削除
		GetEntityManager()->DestroyEntity(gameObject().lock());
	}
}

//========================================
//
// 離れた時
//
//========================================
void DropDeltaScript::OnECSCollisionExit(SphereColliderData* collider)
{
}

