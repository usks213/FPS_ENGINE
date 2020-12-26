//==================================================================
//												CollisionSystem.cpp
//	当たり判定管理システム
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/27	コリジョンシステム作成
//
//===================================================================


//===== インクルード部 =====
#include "CollisionSystem.h"
#include "../ECSCompoent/Collider.h"
#include "../ECS/Entity/IEntity.h"
#include <algorithm>

#include "../main.h"


using namespace ECS;


//===== マクロ定義 =====
#define MAX_CELL (1+4+16+64)


//===== クラス定義 =====
// 四分木空間
class CCell
{
public:
	CCell()
	{

	}
	~CCell()
	{
		m_list.clear();
	}

	void ClearList()
	{
		m_list.clear();
	}

	std::list<Collider*>& GetList() { return m_list; }

	static void SetMapSize(float width, float height)
	{
		m_fUnit_W = width / (float)(1 << (m_uiLevel - 1));
		m_fUnit_H = height / (float)(1 << (m_uiLevel - 1));
	}

	static float GetUnitW() { return m_fUnit_W; }
	static float GetUnitH() { return m_fUnit_H; }
	static unsigned int GetUnitLevel() { return m_uiLevel; }

	std::list<Collider*> m_list;

private:
	static float m_fUnit_W;		// 最小レベル空間の幅単位
	static float m_fUnit_H;		// 最小レベル空間の高単位
	static const unsigned int m_uiLevel = 3;			// 最下位レベル
};

// 初期サイズ
float CCell::m_fUnit_W = 100.0f;		// 最小レベル空間の幅単位
float CCell::m_fUnit_H = 100.0f;		// 最小レベル空間の高単位


//===== プロトタイプ宣言 =====
// ビット分割関数
DWORD BitSeparate32(DWORD n)
{
	n = (n | (n << 8)) & 0x00ff00ff;
	n = (n | (n << 4)) & 0x0f0f0f0f;
	n = (n | (n << 2)) & 0x33333333;
	return (n | (n << 1)) & 0x55555555;
}

// 2Dモートン空間番号算出関数
WORD Get2DMortonNumber(WORD x, WORD y)
{
	return (WORD)(BitSeparate32(x) | (BitSeparate32(y) << 1));
}

// 座標→線形4分木要素番号変換関数
DWORD GetPointElem(float pos_x, float pos_y)
{
	// 本当はフィールドの大きさとか
	return Get2DMortonNumber((WORD)(pos_x / CCell::GetUnitW()), (WORD)(pos_y / CCell::GetUnitH()));
}


//===== グローバル変数 =====



//===================================
//
//	コンストラクタ
//
//===================================
CollisionSystem::CollisionSystem(World* pWorld)
	: System<Collider>(pWorld)
{
	// 更新順
	SetUpdateOrder(UpdateOrder::eCollision);
}


//===================================
//
//	デストラクタ
//
//===================================
CollisionSystem::~CollisionSystem()
{
}


//===================================
//
//	生成時コールバック
//
//===================================
void CollisionSystem::OnCreate()
{
	// マップサイズ // 何故か二倍になってる？　現状9600x9600
	CCell::SetMapSize(100.0f * 8 * 6, 100.0f * 8 * 6);
}

//===================================
//
//	更新時コールバック
//
//===================================
void CollisionSystem::OnUpdate()
{
	// 空間レベルの数
	const unsigned int uiLevel = CCell::GetUnitLevel();
	const unsigned int nMaxCell = MAX_CELL;

	// 空間の作成
	CCell mainCell[MAX_CELL];
	CCell subCell[MAX_CELL];
	// モートン番号
	DWORD Def = 0;
	DWORD wLeftTop = 0;
	DWORD wRightDown = 0;


	// 空間分割 + 状態更新
	std::for_each(m_ComponentList.begin(), m_ComponentList.end(),
		[&](Collider* collider)
		{
			// 実態がない
			if (!collider) return;

			// ここで空間の登録をする
			// 左上と右下を出す
			wLeftTop = GetPointElem(collider->m_bound.GetMin()->x,
				collider->m_bound.GetMin()->z);
			wRightDown = GetPointElem(collider->m_bound.GetMax()->x,
				collider->m_bound.GetMax()->z);
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

			// 今いる空間のメインリストに格納
			mainCell[SpaceNum].GetList().push_back(collider);

			// 今いる空間の親のサブに格納
			while (SpaceNum > 0)
			{
				SpaceNum--;
				SpaceNum /= 4;

				subCell[SpaceNum].GetList().push_back(collider);
			}


			// 過去の状態を保存
			collider->m_bOldState = collider->m_bCurState;
			// 現在の状態を更新
			collider->m_bCurState = false;
		});


	// メイン空間の当たり判定
	for (int i = 0; i < nMaxCell; ++i)
	{
		std::for_each(mainCell[i].GetList().begin(), mainCell[i].GetList().end(),
			[&mainCell, &i](Collider* collider)
			{
				// 当たり判定
				Collision(collider, mainCell[i].GetList());
			});
	}

	// サブ空間の当たり判定
	for (int i = 0; i < nMaxCell; ++i)
	{
		std::for_each(mainCell[i].GetList().begin(), mainCell[i].GetList().end(),
			[&subCell, &i](Collider* collider)
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
void CollisionSystem::OnDestroy()
{
}

//========================================
//
//	ノードとリストの当たり判定
//
//========================================
void CollisionSystem::Collision(Collider* main, const std::list<Collider*>& m_pSubList)
{
	// サブループ
	for (const auto& sub : m_pSubList)
	{
		// 同じだった
		if (main == sub) continue;

		//--- 当たり判定処理 ---
		if (Collider::AABBtoAABB(main, sub))
		{
			// 詳細判定
			if (main->Judgment(sub))
			{
				// 物理？



				// 状態を更新
				main->m_bCurState = true;
				sub->m_bCurState = true;

				//--- 当たり判定コールバック ---
				// Enter
				if (main->m_bCurState && !main->m_bOldState)
				{
					// メッセージ送信
					main->m_Parent.lock()->SendComponentMessage("OnCollisionEnter", sub);
				}
				// Stay
				else if (main->m_bCurState && main->m_bOldState)
				{
					// メッセージ送信
					main->m_Parent.lock()->SendComponentMessage("OnCollisionStay", sub);
				}

				// 相手側
				// Enter
				if (sub->m_bCurState && !sub->m_bOldState)
				{
					// メッセージ送信
					sub->m_Parent.lock()->SendComponentMessage("OnCollisionEnter", main);
				}
				// Stay
				else if (sub->m_bCurState && sub->m_bOldState)
				{
					// メッセージ送信
					sub->m_Parent.lock()->SendComponentMessage("OnCollisionStay", main);
				}
			}
		}

		//--- 当たり判定コールバック ---
		// Exit
		if (!main->m_bCurState && main->m_bOldState)
		{
			// メッセージ送信
			main->m_Parent.lock()->SendComponentMessage("OnCollisionExit", sub);
		}
		// Exit
		if (!sub->m_bCurState && sub->m_bOldState)
		{
			// メッセージ送信
			sub->m_Parent.lock()->SendComponentMessage("OnCollisionExit", main);
		}
	}
}