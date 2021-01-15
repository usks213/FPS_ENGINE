//==================================================================
//								EnemyBaseScript.h
//	エネミーベーススクリプト
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/09	エネミーベースクラス作成
//
//===================================================================


//====== インクルード部 ======

// 自身
#include "EnemyBaseScript.h"

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
#include "EnemyEffectScript.h"


// ネームスペース
using namespace ECS;


//===== マクロ定義 =====



//******************** スクリプトメソッド ********************


//========================================
//
//	開始時
//
//========================================
void EnemyBaseScript::Start()
{
	// 名前・タグ
	gameObject().lock()->SetName("Enemy");
	gameObject().lock()->SetTag("Enemy");

	// 大きさ
	transform().lock()->m_scale = Vector3(200, 200, 200);

	//--- コンポーネンの追加

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
	// リジッドボディ保存
	m_rb = rb;

	// ECSコライダー
	gameObject().lock()->AddComponent<DeltaCollider>()->SetMain(false);


	// 移動速度
	m_speed = 18;
}

//========================================
//
//	更新時
//
//========================================
void EnemyBaseScript::Update()
{
}

//========================================
//
//	後更新時
//
//========================================
void EnemyBaseScript::LateUpdate()
{
}

//========================================
//
//	終了時
//
//========================================
void EnemyBaseScript::End()
{
}


//******************** コールバック関数 ********************

//========================================
//
// 当たった時
//
//========================================
void EnemyBaseScript::OnDeltaCollisionEnter(DeltaCollider* collider)
{
	if (collider->gameObject().lock()->tag() == "Player" || 
		collider->gameObject().lock()->tag() == "Bullet")
	{
		// ドロップアイテムの生成
		const auto& obj = Instantiate<GameObject>(transform().lock()->m_pos);
		obj->AddComponent<DropDeltaScript>()->SetPlayer(m_player.lock());

		// エフェクトの生成
		const auto& effect = Instantiate<GameObject>(transform().lock()->m_pos);
		effect->AddComponent<EnemyEffectScript>();

		// 削除
		Destroy(gameObject().lock());
	}
}

//========================================
//
// 当たっている間
//
//========================================
void EnemyBaseScript::OnDeltaCollisionStay(DeltaCollider* collider)
{
	if (collider->gameObject().lock()->tag() == "Player" ||
		collider->gameObject().lock()->tag() == "Bullet")
	{
		// ドロップアイテムの生成
		const auto& obj = Instantiate<GameObject>(transform().lock()->m_pos);
		obj->AddComponent<DropDeltaScript>()->SetPlayer(m_player.lock());

		// 削除
		Destroy(gameObject().lock());
	}
}

//========================================
//
// 離れた時
//
//========================================
void EnemyBaseScript::OnDeltaCollisionExit(DeltaCollider* collider)
{
}

