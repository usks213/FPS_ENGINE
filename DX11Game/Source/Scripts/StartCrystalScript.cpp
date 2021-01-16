//==================================================================
//								StartCrystalScript.cpp
//	ゲーム開始クリスタル 
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/13	スタートクリスタルスクリプトクラス作成
//
//===================================================================


//====== インクルード部 ======

// 自身
#include "StartCrystalScript.h"

// ワールド
#include "../Engine/ECS/World/WorldManager.h"
#include "../Engine/ECSWorld/GameWorld.h"

// システム
#include "../Engine/System/input.h"
#include "../Engine/System/Sound.h"
#include "../Engine/System/debugproc.h"

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
#include "MakeEnemyScript.h"

// ネームスペース
using namespace ECS;


//===== マクロ定義 =====



//******************** スクリプトメソッド ********************


//========================================
//
//	開始時
//
//========================================
void StartCrystalScript::Start()
{
	// 名前・タグ
	gameObject().lock()->SetName("StartCrystal");
	gameObject().lock()->SetTag("StartCrystal");

	// スケール
	m_fScale = 300.0f;
	transform().lock()->m_scale = Vector3{ m_fScale,m_fScale,m_fScale };
	transform().lock()->m_rot->x = 90;

	//--- コンポーネンの追加

	// インスタンシングレンダラー
	const auto& rn = gameObject().lock()->AddComponent<InstancingMeshRenderer>();
	rn->MakeOctahedron("StartCrystal");
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

	// 継承元
	//BombCrystalScript::Start();

	// 生存時間
	m_nExitTime = 3600;
}

//========================================
//
//	後更新
//
//========================================
void StartCrystalScript::LateUpdate()
{
	Vector3 pos = transform().lock()->m_pos;
}

//========================================
//
//	終了時
//
//========================================
void StartCrystalScript::End()
{
	// 継承元
	BombCrystalScript::End();

	// ゲームスタート
	if (m_bStart)
	{
		const auto& obj = Instantiate<GameObject>();
		const auto& maker = obj->AddComponent<MakeEnemyScript>();
		maker->SetPlayer(m_player.lock());
	}
	// ロードし直し
	else
	{
		WorldManager::GetInstance()->LoadWorld<GameWorld>("Game");
	}
}


//******************** コールバック関数 ********************

//========================================
//
// 当たった時
//
//========================================
void StartCrystalScript::OnDeltaCollisionEnter(DeltaCollider* collider)
{
	// 継承元
	BombCrystalScript::OnDeltaCollisionEnter(collider);

	if (collider->gameObject().lock()->tag() == "Player")
	{
		m_bStart = true;
	}
}

//========================================
//
// 当たっている間
//
//========================================
void StartCrystalScript::OnDeltaCollisionStay(DeltaCollider* collider)
{
	// 継承元
	BombCrystalScript::OnDeltaCollisionStay(collider);

	if (collider->gameObject().lock()->tag() == "Player")
	{
		m_bStart = true;
	}
}
