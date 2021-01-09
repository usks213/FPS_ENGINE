//==================================================================
//								TrackingMoveEnemyHostScript.cpp
//	追尾型エネミーを束ねる親
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/08	トラッキングムーブエネミーホストクラス作成
//
//===================================================================


//====== インクルード部 ======

// 自身
#include "TrackingMoveEnemyHostScript.h"

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
void TrackingMoveEnemyHostScript::Start()
{
	// 名前・タグ
	gameObject().lock()->SetName("TrackingMoveEnemyHost");
	gameObject().lock()->SetTag("Enemy");

	// 大きさ
	transform().lock()->m_scale = Vector3(100, 100, 100);

	//--- コンポーネンの追加

	// インスタンシングレンダラー
	gameObject().lock()->AddComponent<InstancingMeshRenderer>()->MakeDodecahedron("TrackingMoveEnemyHost");

	// ECSリジッドボディ
	const auto& rb = gameObject().lock()->AddComponent<Rigidbody>();
	rb->SetDrag({ 0,0,0 });
	rb->SetGravityForce({ 0,0,0 });
	rb->SetStaticFriction(0);
	rb->SetDynamicFriction(0);
	rb->SetMass(1);
	rb->SetTorqueDrag({ 0,0,0 });
	// リジッドボディ保存
	m_rb = rb;
	// 回転
	Vector3 v = { rand() % 100 / 100.0f, rand() % 100 / 100.0f, rand() % 100 / 100.0f };
	v = v.normalized();
	rb->AddTorque(v * 3);

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
void TrackingMoveEnemyHostScript::Update()
{
	// ターゲットが存在しない
	const auto& target = m_target.lock();
	if (!target) return;

	//--- 追尾処理
	Vector3 dir = target->m_pos - transform().lock()->m_pos;
	dir = dir.normalized();

	// 力を加える
	m_rb.lock()->SetForce(dir * m_speed);

	// 子にも加える
	for (const auto& child_w : m_childList)
	{
		const auto& child_s = child_w.lock();
		if (!child_s) continue;

		// 力を加える
		child_s->GetRb().lock()->SetForce(dir * m_speed);
	}
}

//========================================
//
//	後更新時
//
//========================================
void TrackingMoveEnemyHostScript::LateUpdate()
{
}

//========================================
//
//	終了時
//
//========================================
void TrackingMoveEnemyHostScript::End()
{
}

//========================================
//
//	子の生成
//
//========================================
void TrackingMoveEnemyHostScript::CreateChild(int nNum)
{
	//x = Mathf.Cos(phi) * Mathf.Cos(theta);
	//y = Mathf.Cos(phi) * Mathf.Sin(theta);
	//z = Mathf.Sin(phi);
	//for (int n = 0; n < nNum; ++n)
	//{
	//	for (int i = 0; i < n; ++i)
	//	{
	//		float phi = 360.0f / n * i * (XM_PI / 180.0f);

	//		for (int j = 0; j < n; ++j)
	//		{
	//			float theta = 360.0f / n * j * (XM_PI / 180.0f);
	//			// 座標
	//			Vector3 pos;
	//			pos->x = cosf(phi) * cosf(theta);
	//			pos->y = cosf(phi) * sinf(theta);
	//			pos->z = sinf(phi);
	//			pos *= 100 * n;

	//			// エネミー生成
	//			const auto& obj = Instantiate<GameObject>(pos + transform().lock()->m_pos);
	//			// コンポーネントの追加
	//			const auto& tracking = obj->AddComponent<TrackingMoveEnemyScript>();
	//			// リストへ格納
	//			m_childList.push_back(tracking);
	//		}
	//	}
	//}

	for (int n = 0; n < nNum; n++)
	{
		float phi = rand() % 1000 / 100.0f;

		float theta = rand() % 1000 / 100.0f;
		// 座標
		Vector3 pos;
		pos->x = cosf(phi) * cosf(theta);
		pos->y = sinf(phi);
		pos->z = cosf(phi) * sinf(theta);
		pos->x *= (150 + (n / (n * ((n / 8) + 1) + 1)) * 100);
		pos->z *= (150 + (n / (n * ((n / 8) + 1) + 1)) * 100);
		pos->y *= (150 + (n / (n * ((n / 4) + 1) + 1)) * 100);

		// エネミー生成
		const auto& obj = Instantiate<GameObject>(pos + transform().lock()->m_pos);
		// コンポーネントの追加
		const auto& tracking = obj->AddComponent<TrackingMoveEnemyScript>();
		// リストへ格納
		m_childList.push_back(tracking);
	}
}


//******************** コールバック関数 ********************


//========================================
//
// 当たった時
//
//========================================
void TrackingMoveEnemyHostScript::OnCollisionEnter(Collider* collider)
{

}

//========================================
//
// 当たっている間
//
//========================================
void TrackingMoveEnemyHostScript::OnCollisionStay(Collider* collider)
{

}

//========================================
//
// 離れた時
//
//========================================
void TrackingMoveEnemyHostScript::OnCollisionExit(Collider* collider)
{

}


//===== ECS =====

//========================================
//
// 当たった時
//
//========================================
void TrackingMoveEnemyHostScript::OnDeltaCollisionEnter(DeltaCollider* collider)
{
}

//========================================
//
// 当たっている間
//
//========================================
void TrackingMoveEnemyHostScript::OnDeltaCollisionStay(DeltaCollider* collider)
{
}

//========================================
//
// 離れた時
//
//========================================
void TrackingMoveEnemyHostScript::OnDeltaCollisionExit(DeltaCollider* collider)
{
}

