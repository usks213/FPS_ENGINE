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
	// ベースクラス
	EnemyBaseScript::Start();

	// 名前・タグ
	gameObject().lock()->SetName("TrackingMoveEnemyHost");
	gameObject().lock()->SetTag("Host");

	// 大きさ
	transform().lock()->m_scale = Vector3(100, 100, 100);

	//--- コンポーネンの追加

	// インスタンシングレンダラー
	//gameObject().lock()->AddComponent<InstancingMeshRenderer>()->MakeDodecahedron("TrackingMoveEnemy");

	// 生存時間
	m_nExitTime = 360;
}

//========================================
//
//	更新時
//
//========================================
void TrackingMoveEnemyHostScript::Update()
{
	// ベースクラス
	EnemyBaseScript::Update();

	// 生存時間
	m_nExitTime--;
	if (m_nExitTime < 0)
	{
		// 自身の削除
		GetEntityManager()->DestroyEntity(gameObject().lock());
	}

	// ターゲットが存在しない
	const auto& player = m_player.lock();
	if (!player) return;
	const auto& target = player->transform().lock();

	//--- 追尾処理
	Vector3 dir = target->m_pos - transform().lock()->m_pos;
	if (dir.magnitude() < 500) return;
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
	// ベースクラス
	EnemyBaseScript::LateUpdate();
}

//========================================
//
//	終了時
//
//========================================
void TrackingMoveEnemyHostScript::End()
{
	// ベースクラス
	EnemyBaseScript::End();
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

	int j = 0;
	int k = 0;
	for (int n = 0; n < nNum; n++)
	{
		float phi = rand() % 1000 / 100.0f;

		float theta = rand() % 1000 / 100.0f;
		// 座標
		Vector3 pos;
		pos->x = cosf(phi) * cosf(theta);
		pos->y = sinf(phi);
		pos->z = cosf(phi) * sinf(theta);

		if (n > j * 4)
		{
			k++;
			j += k;
		}
		pos->x *= (150 + k * 150);
		pos->z *= (150 + k * 150);
		pos->y *= (150 + k * 2 * 150);

		// エネミー生成
		const auto& obj = Instantiate<GameObject>(pos + transform().lock()->m_pos);
		// コンポーネントの追加
		const auto& tracking = obj->AddComponent<TrackingMoveEnemyScript>();
		tracking->SetPlayer(m_player.lock());
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
