//==================================================================
//												DeltaDeltaCollisionSystem.cpp
//	デルタ専用当たり判定管理システム
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/09	デルタコリジョンシステム作成
//
//===================================================================


//===== インクルード部 =====
#include "DeltaCollisionSystem.h"
#include "../ECSCompoent/DeltaCollider.h"
#include "../ECS/Entity/IEntity.h"
#include <algorithm>

#include "../System/CCell.h"
#include "../ECS/Entity/EntityManager.h"

#include "../main.h"
#include "../ECSCompoent/Transform.h"
#include "../Renderer/Camera.h"
#include "../ECSEntity/GameObject.h"

using namespace ECS;


//===== マクロ定義 =====

//===== グローバル変数 =====



//===================================
//
//	コンストラクタ
//
//===================================
DeltaCollisionSystem::DeltaCollisionSystem(World* pWorld)
	: System<DeltaCollider>(pWorld)
{
	// 更新順
	SetUpdateOrder(UpdateOrder::eCollision);
}


//===================================
//
//	デストラクタ
//
//===================================
DeltaCollisionSystem::~DeltaCollisionSystem()
{
}


//===================================
//
//	生成時コールバック
//
//===================================
void DeltaCollisionSystem::OnCreate()
{
	// マップサイズ // 何故か二倍になってる？　現状4000x4000
	CCell<DeltaCollider>::SetMapSize(100.0f * 5 * 10, 100.0f * 5 * 10);
}

//===================================
//
//	更新時コールバック
//
//===================================
void DeltaCollisionSystem::OnUpdate()
{
	// 空間レベルの数
	const unsigned int uiLevel = CCell<DeltaCollider>::GetUnitLevel();
	const unsigned int nMaxCell = MAX_CELL;

	// 空間の作成
	CCell<DeltaCollider> mainMainCell[MAX_CELL];
	CCell<DeltaCollider> mainCell[MAX_CELL];
	CCell<DeltaCollider> subCell[MAX_CELL];
	// モートン番号
	DWORD Def = 0;
	DWORD wLeftTop = 0;
	DWORD wRightDown = 0;
	// カメラ座標
	Vector3 cameraPos = CCamera::GetMainCamera()->GetPos();
	// オフセット
	CCell<DeltaCollider>::SetOffSet(cameraPos->x, cameraPos->z);

	// 空間分割 + 状態更新
	std::for_each(m_ComponentList.begin(), m_ComponentList.end(),
		[&](DeltaCollider* collider)
		{
			// 中心座標
			Vector3 boxPos1 = collider->m_transform.lock()->m_pos + collider->m_bound.GetCenter();
			// ボックスの最大最小
			Vector3 boxMax1 = boxPos1 + Vector3(collider->m_bound.GetMax() * collider->m_transform.lock()->m_scale);
			Vector3 boxMin1 = boxPos1 + Vector3(collider->m_bound.GetMin() * collider->m_transform.lock()->m_scale);

			// ここで空間の登録をする
			// 左上と右下を出す
			wLeftTop = CCell<DeltaCollider>::GetPointElem(boxMin1->x, boxMin1->z);
			wRightDown = CCell<DeltaCollider>::GetPointElem(boxMax1->x, boxMax1->z);
			if (wLeftTop >= nMaxCell - 1 || wRightDown >= nMaxCell - 1)
			{
				// 削除
				GetEntityManager()->DestroyEntity(collider->gameObject().lock());
				return;
			}

			// XORをとる	
			Def = wLeftTop ^ wRightDown;
			unsigned int HiLevel = 0;
			unsigned int i;
			for (i = 0; i < uiLevel; i++)
			{
				DWORD Check = (Def >> (i * 2)) & 0x3;
				if (Check != 0)
					HiLevel = i + 1;
			}
			DWORD SpaceNum = wRightDown >> (HiLevel * 2);
			int nPow4 = 1;
			for (i = 0; i < uiLevel - HiLevel; i++) nPow4 *= 4;
			DWORD AddNum = (nPow4 - 1) / 3;
			SpaceNum += AddNum;	// これが今いる空間

			// 空間外ははじく
			if (SpaceNum > nMaxCell - 1)
			{
				// 削除
				GetEntityManager()->DestroyEntity(collider->gameObject().lock());
				return;
			}

			// メインコライダーか
			if (collider->m_bMain)
			{
				// 今いる空間のメインリストに格納
				mainMainCell[SpaceNum].GetList().push_back(collider);
			}
			else
			{
				// 今いる空間のメインリストに格納
				mainCell[SpaceNum].GetList().push_back(collider);

				// 今いる空間の親のサブに格納
				while (SpaceNum > 0)
				{
					SpaceNum--;
					SpaceNum /= 4;

					subCell[SpaceNum].GetList().push_back(collider);
				}
			}

			// 過去の状態を保存
			collider->m_bOldState = collider->m_bCurState;
			// 現在の状態を更新
			collider->m_bCurState = false;
		});


	// メイン空間の当たり判定
	for (int i = 0; i < nMaxCell; ++i)
	{
		std::for_each(mainMainCell[i].GetList().begin(), mainMainCell[i].GetList().end(),
			[&mainCell, &i](DeltaCollider* collider)
			{
				// 当たり判定
				Collision(collider, mainCell[i].GetList());
			});
	}

	// サブ空間の当たり判定
	for (int i = 0; i < nMaxCell; ++i)
	{
		std::for_each(mainMainCell[i].GetList().begin(), mainMainCell[i].GetList().end(),
			[&subCell, &i](DeltaCollider* collider)
			{
				// 当たり判定
				Collision(collider, subCell[i].GetList());
			});
	}
}

//===================================
//
//	削除時コールバック
//
//===================================
void DeltaCollisionSystem::OnDestroy()
{
}

//========================================
//
//	ノードとリストの当たり判定
//
//========================================
void DeltaCollisionSystem::Collision(DeltaCollider* main, const std::list<DeltaCollider*>& m_pSubList)
{
	// サブループ
	for (const auto& sub : m_pSubList)
	{
		// 同じだった
		if (main == sub) continue;

		//--- 当たり判定処理 ---
		if (DeltaCollider::SpheretoSphere(main, sub))
		{
			// 状態を更新
			main->m_bCurState = true;
			sub->m_bCurState = true;

			//--- 当たり判定コールバック ---
			// Enter
			if (main->m_bCurState && !main->m_bOldState)
			{
				// メッセージ送信
				main->m_Parent.lock()->SendComponentMessage("OnDeltaCollisionEnter", sub);
			}
			// Stay
			else if (main->m_bCurState && main->m_bOldState)
			{
				// メッセージ送信
				main->m_Parent.lock()->SendComponentMessage("OnDeltaCollisionStay", sub);
			}

			// 相手側
			// Enter
			if (sub->m_bCurState && !sub->m_bOldState)
			{
				// メッセージ送信
				sub->m_Parent.lock()->SendComponentMessage("OnDeltaCollisionEnter", main);
			}
			// Stay
			else if (sub->m_bCurState && sub->m_bOldState)
			{
				// メッセージ送信
				sub->m_Parent.lock()->SendComponentMessage("OnDeltaCollisionStay", main);
			}
		}

		//--- 当たり判定コールバック ---
		// Exit
		if (!main->m_bCurState && main->m_bOldState)
		{
			// メッセージ送信
			main->m_Parent.lock()->SendComponentMessage("OnDeltaCollisionExit", sub);
		}
		// Exit
		if (!sub->m_bCurState && sub->m_bOldState)
		{
			// メッセージ送信
			sub->m_Parent.lock()->SendComponentMessage("OnDeltaCollisionExit", main);
		}
	}
}