//==================================================================
//								MakeEnemyScript.h
//	エネミー生成スクリプト
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/08	エネミー生成スクリプトクラス作成
//
//===================================================================


//====== インクルード部 ======

// 自身
#include "MakeEnemyScript.h"

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
#include "../Engine/ECSCompoent/Rigidbody.h"
#include "../Engine/ECSCompoent/DeltaCollider.h"

// ECSシステム
#include "../Engine/ECSSystem/RigidbodySystem.h"
#include "../Engine/ECSSystem/DeltaCollisionSystem.h"

// スクリプト
#include "StraightMoveEnemyScript.h"
#include "TrackingMoveEnemyHostScript.h"
#include "PlayerScript.h"
#include "BombCrystalScript.h"



// ネームスペース
using namespace ECS;


//===== マクロ定義 =====
#define MAX_SPAWN_NUM (50)


//******************** スクリプトメソッド ********************


//========================================
//
//	開始時
//
//========================================
void MakeEnemyScript::Start()
{
	// 初期の生成タイミング
	m_nSpawnInterval = 60;
	m_nSpawnCount = m_nSpawnInterval;
	// 初期の生成数
	m_nSpawnNum = 1;
	// 生成タイプ
	m_nType = 0;
}

//========================================
//
//	更新時
//
//========================================
void MakeEnemyScript::Update()
{
	//----- プレイヤーと同期 -----

	// プレイヤーを取得
	const auto& player = m_player.lock();
	if (!player) return;

	// プレイヤー座標
	const auto& playerTrans = player->transform().lock();
	Vector3 playerPos = player->transform().lock()->m_pos;
	// 座標の同期
	transform().lock()->m_pos = playerPos;


	//----- エネミーの生成 -----

	// 生成カウント
	m_nSpawnCount--;
	if (m_nSpawnCount >= 0) return;

	// カウント初期化
	m_nSpawnCount = m_nSpawnInterval;

	// デルタ数を取得
	int nDelta = player->GetComponent<PlayerScript>()->GetDeltaCount();
	for(int i = 1; i < m_nSpawnNum; i++)
		nDelta -= i * 10;
	// 生成数を計算
	if (nDelta / (m_nSpawnNum * 10))
	{
		m_nSpawnNum++;
	}
	if (m_nSpawnNum > MAX_SPAWN_NUM) m_nSpawnNum = MAX_SPAWN_NUM;

	// 生成するエネミーの種類
	//int type = rand() % eMaxType;
	//m_nType = (m_nType +1) % 3;
	for (m_nType = 0; m_nType < eMaxType; m_nType++)
	{
		//--- エネミーを生成する座標
		float height = (rand() % 5 - 2) * 100;
		if (playerPos->y < 600) playerPos->y = 100;
		Vector3 spawnPos = { 3000, height, 0 };
		Vector3 temp = playerPos;
		//temp->y = 0;
		spawnPos = spawnPos.RotationY(rand() % 360) + temp;

		//--- ボムクリスタルの生成
		if (rand() % 100 < 30)
		{
			Vector3 bombPos = spawnPos * 0.75f;
			bombPos->y = playerPos->y + (5) * 100;
			const auto& bomb = Instantiate<GameObject>(bombPos);
			bomb->AddComponent<BombCrystalScript>();
		}

		// エンティティの数
		if (GetEntityManager()->GetEntityCount() > 10000) break;

		//--- タイプで分ける
		switch (m_nType)
		{
			// 直進タイプ 波紋
		case ECS::MakeEnemyScript::eStraightCircle:
		{
			// 生成数
			int SpawnNum = m_nSpawnNum + 3;

			// 生成座標
			Vector3 pos = (playerPos - spawnPos).normalized() * SpawnNum * 200;
			pos->y = 0;
			Vector3 backPos = pos;
			// 生成数の半分だけ角度をずらす
			float addAngle = 45.0f / SpawnNum;
			pos = pos.RotationY(-addAngle * (SpawnNum / 2));
			Vector3 center = spawnPos;
			center->y = 0;

			for (int i = 0; i < SpawnNum; i++)
			{
				// エネミー生成
				const auto& obj = Instantiate<GameObject>(pos.RotationY(addAngle * i) - backPos + spawnPos);
				// コンポーネントの追加
				obj->AddComponent<StraightMoveEnemyScript>()->SetPlayer(player);
				// 進む向き
				const auto& rb = obj->GetComponent<Rigidbody>();
				Vector3 dir = (temp - center).normalized();
				rb->AddForce(dir * 20);
			}
		}
		break;
		// 直進タイプ タワー
		case ECS::MakeEnemyScript::eStraightTower:
		{
			// 生成数
			int SpawnNum = m_nSpawnNum / 2 + 3;

			for (int y = 0; y < SpawnNum / 3; y++)
				for (int x = 0; x < SpawnNum; x += 6)
					for (int i = 0; i < x; i++)
					{
						// 生成座標
						Vector3 pos = { x * 40.0f, 0, 0 };
						float angle = 360.0f / x * i;
						pos = pos.RotationY(angle);
						Vector3 center = spawnPos;
						center->y += y * 200;
						// エネミー生成
						const auto& obj = Instantiate<GameObject>(pos + center);
						// コンポーネントの追加
						obj->AddComponent<StraightMoveEnemyScript>()->SetPlayer(player);
						// 進む向き
						const auto& rb = obj->GetComponent<Rigidbody>();
						center->y = 0;
						Vector3 dir = (temp - center).normalized();
						rb->AddForce(dir * 20);
					}
		}
		break;
		case ECS::MakeEnemyScript::eTracking:
		{
			// エネミー生成
			const auto& obj = Instantiate<GameObject>(spawnPos);
			// コンポーネントの追加
			const auto& tracking = obj->AddComponent<TrackingMoveEnemyHostScript>();
			tracking->SetPlayer(player);

			// 生成数
			int SpawnNum = m_nSpawnNum * 1.5f + 3;

			// 子の生成
			tracking->CreateChild(SpawnNum);
		}
		break;
		case ECS::MakeEnemyScript::eGroup:
			break;
		case ECS::MakeEnemyScript::eMaxType:
			break;
		}
	}
}

//========================================
//
//	後更新時
//
//========================================
void MakeEnemyScript::LateUpdate()
{
	// デバック表示
	PrintDebugProc("SpawnNum:%d\n\n", m_nSpawnNum);
	PrintDebugProc("EntityNum:%d\n", GetEntityManager()->GetEntityCount());
}

//========================================
//
//	終了時
//
//========================================
void MakeEnemyScript::End()
{
}
