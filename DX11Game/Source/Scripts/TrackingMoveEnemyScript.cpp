//==================================================================
//								TrackingMoveEnemyScript.cpp
//	追尾型エネミー
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/08	トラッキングムーブエネミークラス作成
//
//===================================================================


//====== インクルード部 ======

// 自身
#include "TrackingMoveEnemyScript.h"

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
void TrackingMoveEnemyScript::Start()
{
	// 名前・タグ
	gameObject().lock()->SetName("TrackingMoveEnemy");
	gameObject().lock()->SetTag("Enemy");

	// 大きさ
	transform().lock()->m_scale = Vector3(200, 200, 200);

	//--- コンポーネンの追加

	// インスタンシングレンダラー
	gameObject().lock()->AddComponent<InstancingMeshRenderer>()->MakeDodecahedron("TrackingMoveEnemy");

	// ECSリジッドボディ
	const auto& rb = gameObject().lock()->AddComponent<ECSRigidbody>();
	rb->GetData()->SetDrag({ 0,0,0 });
	rb->GetData()->SetGravityForce({ 0,0,0 });
	rb->GetData()->SetStaticFriction(0);
	rb->GetData()->SetDynamicFriction(0);
	rb->GetData()->SetMass(1);
	rb->GetData()->SetTorqueDrag({ 0,0,0 });
	// リジッドボディ保存
	m_rb = rb;

	// ECSコライダー
	gameObject().lock()->AddComponent<ECSSphereCollider>()->GetData()->SetMain(false);

}

//========================================
//
//	更新時
//
//========================================
void TrackingMoveEnemyScript::Update()
{
}

//========================================
//
//	後更新時
//
//========================================
void TrackingMoveEnemyScript::LateUpdate()
{
}

//========================================
//
//	終了時
//
//========================================
void TrackingMoveEnemyScript::End()
{
}


//******************** コールバック関数 ********************


//========================================
//
// 当たった時
//
//========================================
void TrackingMoveEnemyScript::OnCollisionEnter(Collider* collider)
{

}

//========================================
//
// 当たっている間
//
//========================================
void TrackingMoveEnemyScript::OnCollisionStay(Collider* collider)
{

}

//========================================
//
// 離れた時
//
//========================================
void TrackingMoveEnemyScript::OnCollisionExit(Collider* collider)
{

}


//===== ECS =====

//========================================
//
// 当たった時
//
//========================================
void TrackingMoveEnemyScript::OnECSCollisionEnter(SphereColliderData* collider)
{
}

//========================================
//
// 当たっている間
//
//========================================
void TrackingMoveEnemyScript::OnECSCollisionStay(SphereColliderData* collider)
{
}

//========================================
//
// 離れた時
//
//========================================
void TrackingMoveEnemyScript::OnECSCollisionExit(SphereColliderData* collider)
{
}

