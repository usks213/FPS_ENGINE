//==================================================================
//								BombCrystalScript.cpp
//	ボム生成アイテム 
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/10	ボムクリスタルスクリプトクラス作成
//
//===================================================================


//====== インクルード部 ======

// 自身
#include "BombCrystalScript.h"

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
#include "BombEffectScript.h"
#include "BombCollisionScript.h"


// ネームスペース
using namespace ECS;


//===== マクロ定義 =====



//******************** スクリプトメソッド ********************


//========================================
//
//	開始時
//
//========================================
void BombCrystalScript::Start()
{
	// 名前・タグ
	gameObject().lock()->SetName("BombCrystal");
	gameObject().lock()->SetTag("BombCrystal");

	// 大きさ
	transform().lock()->m_scale = Vector3(100, 100, 100);
	transform().lock()->m_rot->x = 90;

	//--- コンポーネンの追加

	// インスタンシングレンダラー
	const auto& rn = gameObject().lock()->AddComponent<InstancingMeshRenderer>();
	rn->MakeOctahedron("BombCrystal");
	rn->SetDiffuseColor({ 1,0,1,1 });

	// ECSリジッドボディ
	const auto& rb = gameObject().lock()->AddComponent<Rigidbody>();
	rb->SetDrag({ 0,0,0 });
	rb->SetGravityForce({ 0,0,0 });
	rb->SetStaticFriction(0);
	rb->SetDynamicFriction(0);
	rb->SetMass(10);
	rb->SetTorqueDrag({ 0,0,0 });

	// ECSコライダー
	gameObject().lock()->AddComponent<DeltaCollider>()->SetMain(false);


	// 生存時間
	m_nExitTime = 1800;
}

//========================================
//
//	更新時
//
//========================================
void BombCrystalScript::Update()
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
void BombCrystalScript::LateUpdate()
{
}

//========================================
//
//	終了時
//
//========================================
void BombCrystalScript::End()
{
}


//******************** コールバック関数 ********************

//========================================
//
// 当たった時
//
//========================================
void BombCrystalScript::OnDeltaCollisionEnter(DeltaCollider* collider)
{
	if (collider->gameObject().lock()->tag() == "Player")
	{
		// ボムエフェクト生成
		const int n = 24;
		//const float f = rand() % 314 / 100.0f;
		for (int i = 0; i < n; ++i)
		{
			float phi = 360.0f / n * i * (XM_PI / 180.0f);

			for (int j = 0; j < n; ++j)
			{
				float theta = 360.0f / n * j * (XM_PI / 180.0f);
				// 座標
				Vector3 pos;
				pos->x = cosf(phi) * cosf(theta);
				pos->y = cosf(phi) * sinf(theta);
				pos->z = sinf(phi);
				pos *= 100;

				// エフェクト生成
				const auto& obj = Instantiate<GameObject>(pos + transform().lock()->m_pos);
				// コンポーネントの追加
				const auto& effect = obj->AddComponent<BombEffectScript>();
				// リジッドボディ
				const auto& rb = obj->GetComponent<Rigidbody>();
				rb->AddForce(pos.normalized() * 30);
			}
		}

		// 当たり判定範囲生成
		const auto& obj = Instantiate<GameObject>(transform().lock()->m_pos);
		// コンポーネントの追加
		const auto& col = obj->AddComponent<BombCollisionScript>();

		// 自身の破棄
		Destroy(gameObject().lock());
	}
}

//========================================
//
// 当たっている間
//
//========================================
void BombCrystalScript::OnDeltaCollisionStay(DeltaCollider* collider)
{
	if (collider->gameObject().lock()->tag() == "Player")
	{
		// ボムエフェクト生成
		const int n = 24;
		//const float f = rand() % 314 / 100.0f;
		for (int i = 0; i < n; ++i)
		{
			float phi = 360.0f / n * i * (XM_PI / 180.0f);

			for (int j = 0; j < n; ++j)
			{
				float theta = 360.0f / n * j * (XM_PI / 180.0f);
				// 座標
				Vector3 pos;
				pos->x = cosf(phi) * cosf(theta);
				pos->y = cosf(phi) * sinf(theta);
				pos->z = sinf(phi);
				pos *= 100;

				// エフェクト生成
				const auto& obj = Instantiate<GameObject>(pos + transform().lock()->m_pos);
				// コンポーネントの追加
				const auto& effect = obj->AddComponent<BombEffectScript>();
				// リジッドボディ
				const auto& rb = obj->GetComponent<Rigidbody>();
				rb->AddForce(pos.normalized() * 30);
			}
		}

		// 当たり判定範囲生成
		const auto& obj = Instantiate<GameObject>(transform().lock()->m_pos);
		// コンポーネントの追加
		const auto& col = obj->AddComponent<BombCollisionScript>();

		// 自身の破棄
		Destroy(gameObject().lock());
	}
}

//========================================
//
// 離れた時
//
//========================================
void BombCrystalScript::OnDeltaCollisionExit(DeltaCollider* collider)
{
}

