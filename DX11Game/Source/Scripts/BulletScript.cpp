//==================================================================
//								BulletScript.cpp
//	バレットスクリプト
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/07	バレットスクリプトクラス作成
//
//===================================================================


//====== インクルード部 ======

// 自身
#include "BulletScript.h"

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
void BulletScript::Start()
{
	// 名前・タグ
	gameObject().lock()->SetName("Bullet");
	gameObject().lock()->SetTag("Bullet");

	// 大きさ
	transform().lock()->m_scale = Vector3(1000, 1000, 1000);

	//--- コンポーネンの追加

	// インスタンシングレンダラー
	gameObject().lock()->AddComponent<InstancingMeshRenderer>()->MakeDodecahedron("Bullet");

	// ECSリジッドボディ
	const auto& rb = gameObject().lock()->AddComponent<ECSRigidbody>();
	rb->GetData()->SetDrag({ 0,0,0 });
	rb->GetData()->SetGravityForce({ 0,0,0 });
	rb->GetData()->SetStaticFriction(0);
	rb->GetData()->SetDynamicFriction(0);
	rb->GetData()->SetMass(10);
	rb->GetData()->SetTorqueDrag({ 0,0,0 });

	// ECSコライダー
	gameObject().lock()->AddComponent<ECSSphereCollider>();


	// 生存時間
	m_nExitTime = 300;
}

//========================================
//
//	更新時
//
//========================================
void BulletScript::Update()
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
void BulletScript::LateUpdate()
{
}

//========================================
//
//	終了時
//
//========================================
void BulletScript::End()
{
}


//******************** コールバック関数 ********************


//========================================
//
// 当たった時
//
//========================================
void BulletScript::OnCollisionEnter(Collider* collider)
{

}

//========================================
//
// 当たっている間
//
//========================================
void BulletScript::OnCollisionStay(Collider* collider)
{

}

//========================================
//
// 離れた時
//
//========================================
void BulletScript::OnCollisionExit(Collider* collider)
{

}


//===== ECS =====

//========================================
//
// 当たった時
//
//========================================
void BulletScript::OnECSCollisionEnter(SphereColliderData* collider)
{
	if (collider->gameObject().lock()->tag() != "Player")
	{
		// 削除
		GetEntityManager()->DestroyEntity(collider->gameObject().lock());
		// 自身の削除
		//GetEntityManager()->DestroyEntity(gameObject().lock());
	}
}

//========================================
//
// 当たっている間
//
//========================================
void BulletScript::OnECSCollisionStay(SphereColliderData* collider)
{
	if (collider->gameObject().lock()->tag() != "Player")
	{
		// 削除
		GetEntityManager()->DestroyEntity(collider->gameObject().lock());
		// 自身の削除
		//GetEntityManager()->DestroyEntity(gameObject().lock());
	}
}

//========================================
//
// 離れた時
//
//========================================
void BulletScript::OnECSCollisionExit(SphereColliderData* collider)
{
}

