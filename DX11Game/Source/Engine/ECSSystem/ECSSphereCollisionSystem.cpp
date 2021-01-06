//==================================================================
//												ECSSphereCollisionSystem.cpp
//	データ指向版球当たり判定管理システム
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/01/06	データ指向版で作成
//
//===================================================================


//===== インクルード部 =====
#include "ECSSphereCollisionSystem.h"
#include "../ECSCompoent/ECSSphereCollider.h"
#include <algorithm>
#include "../main.h"
#include "../ECSCompoent/Transform.h"
#include "../ECS/Entity/IEntity.h"
#include "../System/CCell.h"

#include "../Renderer/Camera.h"

using namespace ECS;


//===== マクロ定義 =====



//===================================
//
//	コンストラクタ
//
//===================================
ECSSphereCollisionSystem::ECSSphereCollisionSystem(World* pWorld)
	: ECSSystem<SphereColliderData>(pWorld)
{
	// 更新順
	SetUpdateOrder(UpdateOrder::eCollision);
}


//===================================
//
//	デストラクタ
//
//===================================
ECSSphereCollisionSystem::~ECSSphereCollisionSystem()
{
}


//===================================
//
//	生成時コールバック
//
//===================================
void ECSSphereCollisionSystem::OnCreate()
{
	// マップサイズ // 何故か二倍になってる？　現状9600x9600
	CCell<SphereColliderData>::SetMapSize(100.0f * 8 * 10, 100.0f * 8 * 10);
}

//===================================
//
//	更新時コールバック
//
//===================================
void ECSSphereCollisionSystem::OnUpdate()
{
	// ポインタの更新
	ReLoadComponentData();

	// 空間レベルの数
	const unsigned int uiLevel = CCell<SphereColliderData>::GetUnitLevel();
	const unsigned int nMaxCell = MAX_CELL;

	// 空間の作成
	CCell<SphereColliderData> mainMainCell[MAX_CELL];
	CCell<SphereColliderData> mainCell[MAX_CELL];
	CCell<SphereColliderData> subCell[MAX_CELL];
	// モートン番号
	DWORD Def = 0;
	DWORD wLeftTop = 0;
	DWORD wRightDown = 0;
	// カメラ座標
	Vector3 cameraPos = CCamera::GetMainCamera()->GetPos();
	// オフセット
	CCell<SphereColliderData>::SetOffSet(cameraPos->x, cameraPos->z);

	// 空間分割 + 状態更新
	std::for_each(m_ComponentList.begin(), m_ComponentList.end(),
		[&](SphereColliderData& collider)
		{
			// 中心座標
			Vector3 boxPos1 = collider.m_transform.lock()->m_pos + collider.m_bound.GetCenter();
			// ボックスの最大最小
			Vector3 boxMax1 = boxPos1 + Vector3(collider.m_bound.GetMax() * collider.m_transform.lock()->m_scale);
			Vector3 boxMin1 = boxPos1 + Vector3(collider.m_bound.GetMin() * collider.m_transform.lock()->m_scale);

			// ここで空間の登録をする
			// 左上と右下を出す
			wLeftTop = CCell<SphereColliderData>::GetPointElem(boxMin1->x, boxMin1->z);
			wRightDown = CCell<SphereColliderData>::GetPointElem(boxMax1->x, boxMax1->z);
			if (wLeftTop >= nMaxCell - 1 || wRightDown >= nMaxCell - 1) return;

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
			if (SpaceNum > nMaxCell - 1) return;

			// メインコライダーか
			if (collider.m_bMain)
			{
				// 今いる空間のメインリストに格納
				mainMainCell[SpaceNum].GetList().push_back(&collider);
			}
			else
			{
				// 今いる空間のメインリストに格納
				mainCell[SpaceNum].GetList().push_back(&collider);

				// 今いる空間の親のサブに格納
				while (SpaceNum > 0)
				{
					SpaceNum--;
					SpaceNum /= 4;

					subCell[SpaceNum].GetList().push_back(&collider);
				}
			}

			// 過去の状態を保存
			collider.m_bOldState = collider.m_bCurState;
			// 現在の状態を更新
			collider.m_bCurState = false;
		});


	// メイン空間の当たり判定
	for (int i = 0; i < nMaxCell; ++i)
	{
		std::for_each(mainMainCell[i].GetList().begin(), mainMainCell[i].GetList().end(),
			[&mainCell, &i](SphereColliderData* collider)
			{
				// 当たり判定
				Collision(collider, mainCell[i].GetList());
			});
	}

	// サブ空間の当たり判定
	for (int i = 0; i < nMaxCell; ++i)
	{
		std::for_each(mainMainCell[i].GetList().begin(), mainMainCell[i].GetList().end(),
			[&subCell, &i](SphereColliderData* collider)
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
void ECSSphereCollisionSystem::OnDestroy()
{
}

//========================================
//
//	ノードとリストの当たり判定
//
//========================================
void ECSSphereCollisionSystem::Collision(SphereColliderData* main, const std::list<SphereColliderData*>& m_pSubList)
{
	// サブループ
	for (const auto& sub : m_pSubList)
	{
		// 同じだった
		if (main == sub) continue;

		//--- 当たり判定処理 ---
		if (SphereColliderData::SpheretoSphere(main, sub))
		{
			// 状態を更新
			main->m_bCurState = true;
			sub->m_bCurState = true;

			//--- 当たり判定コールバック ---
			// Enter
			if (main->m_bCurState && !main->m_bOldState)
			{
				// メッセージ送信
				main->m_Parent.lock()->SendComponentMessage("OnECSCollisionEnter", sub);
			}
			// Stay
			else if (main->m_bCurState && main->m_bOldState)
			{
				// メッセージ送信
				main->m_Parent.lock()->SendComponentMessage("OnECSCollisionStay", sub);
			}

			// 相手側
			// Enter
			if (sub->m_bCurState && !sub->m_bOldState)
			{
				// メッセージ送信
				sub->m_Parent.lock()->SendComponentMessage("OnECSCollisionEnter", main);
			}
			// Stay
			else if (sub->m_bCurState && sub->m_bOldState)
			{
				// メッセージ送信
				sub->m_Parent.lock()->SendComponentMessage("OnECSCollisionStay", main);
			}
		}

		//--- 当たり判定コールバック ---
		// Exit
		if (!main->m_bCurState && main->m_bOldState)
		{
			// メッセージ送信
			main->m_Parent.lock()->SendComponentMessage("OnECSCollisionExit", sub);
		}
		// Exit
		if (!sub->m_bCurState && sub->m_bOldState)
		{
			// メッセージ送信
			sub->m_Parent.lock()->SendComponentMessage("OnECSCollisionExit", main);
		}
	}
}