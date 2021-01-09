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
void BulletScript::Start()
{
	// 名前・タグ
	gameObject().lock()->SetName("Bullet");
	gameObject().lock()->SetTag("Bullet");

	// 大きさ
	transform().lock()->m_scale = Vector3(300, 300, 300);

	//--- コンポーネンの追加

	// インスタンシングレンダラー
	gameObject().lock()->AddComponent<InstancingMeshRenderer>()->MakeIcosahedron("Bullet");

	// ECSリジッドボディ
	const auto& rb = gameObject().lock()->AddComponent<Rigidbody>();
	rb->SetDrag({ 0,0,0 });
	rb->SetGravityForce({ 0,0,0 });
	rb->SetStaticFriction(0);
	rb->SetDynamicFriction(0);
	rb->SetMass(10);
	rb->SetTorqueDrag({ 0,0,0 });

	// ECSコライダー
	gameObject().lock()->AddComponent<DeltaCollider>();


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
void BulletScript::OnDeltaCollisionEnter(DeltaCollider* collider)
{
	if (collider->gameObject().lock()->tag() == "Enemy")
	{

	}
}

//========================================
//
// 当たっている間
//
//========================================
void BulletScript::OnDeltaCollisionStay(DeltaCollider* collider)
{
	if (collider->gameObject().lock()->tag() == "Enemy")
	{

	}
}

//========================================
//
// 離れた時
//
//========================================
void BulletScript::OnDeltaCollisionExit(DeltaCollider* collider)
{
}

