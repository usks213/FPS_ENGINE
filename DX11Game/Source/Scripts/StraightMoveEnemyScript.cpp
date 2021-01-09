//==================================================================
//								StraightMoveEnemyScript.cpp
//	ストレートムーブエネミー
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/08	ストレートムーブエネミークラス作成
//
//===================================================================


//====== インクルード部 ======

// 自身
#include "StraightMoveEnemyScript.h"

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



//******************** スクリプトメソッド ********************


//========================================
//
//	開始時
//
//========================================
void StraightMoveEnemyScript::Start()
{
	// 名前・タグ
	gameObject().lock()->SetName("StraightMoveEnemy");
	gameObject().lock()->SetTag("Enemy");

	// 大きさ
	transform().lock()->m_scale = Vector3(200, 200, 200);

	//--- コンポーネンの追加

	// インスタンシングレンダラー
	gameObject().lock()->AddComponent<InstancingMeshRenderer>()->MakeOctahedron("StraightMoveEnemy");

	// ECSリジッドボディ
	const auto& rb = gameObject().lock()->AddComponent<Rigidbody>();
	rb->SetDrag({ 0,0,0 });
	rb->SetGravityForce({ 0,0,0 });
	rb->SetStaticFriction(0);
	rb->SetDynamicFriction(0);
	rb->SetMass(1);
	rb->SetTorqueDrag({ 0,0,0 });
	// 回転
	Vector3 v = { rand() % 100 / 100.0f, rand() % 100 / 100.0f, rand() % 100 / 100.0f };
	v = v.normalized();
	rb->AddTorque(v * 3);

	// ECSコライダー
	gameObject().lock()->AddComponent<DeltaCollider>()->SetMain(false);
}

//========================================
//
//	更新時
//
//========================================
void StraightMoveEnemyScript::Update()
{
}

//========================================
//
//	後更新時
//
//========================================
void StraightMoveEnemyScript::LateUpdate()
{
}

//========================================
//
//	終了時
//
//========================================
void StraightMoveEnemyScript::End()
{
}


//******************** コールバック関数 ********************


//========================================
//
// 当たった時
//
//========================================
void StraightMoveEnemyScript::OnCollisionEnter(Collider* collider)
{

}

//========================================
//
// 当たっている間
//
//========================================
void StraightMoveEnemyScript::OnCollisionStay(Collider* collider)
{

}

//========================================
//
// 離れた時
//
//========================================
void StraightMoveEnemyScript::OnCollisionExit(Collider* collider)
{

}


//===== ECS =====

//========================================
//
// 当たった時
//
//========================================
void StraightMoveEnemyScript::OnDeltaCollisionEnter(DeltaCollider* collider)
{
}

//========================================
//
// 当たっている間
//
//========================================
void StraightMoveEnemyScript::OnDeltaCollisionStay(DeltaCollider* collider)
{
}

//========================================
//
// 離れた時
//
//========================================
void StraightMoveEnemyScript::OnDeltaCollisionExit(DeltaCollider* collider)
{
}

