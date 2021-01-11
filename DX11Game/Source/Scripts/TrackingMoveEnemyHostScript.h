//==================================================================
//								TrackingMoveEnemyHostScript.h
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

#pragma once

//====== インクルード部 ======
#include "EnemyBaseScript.h"

#include "TrackingMoveEnemyScript.h"

//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	class TrackingMoveEnemyHostScript : public EnemyBaseScript
	{
	private:
		// ----- メンバ -----
		// 子リスト
		std::list<std::weak_ptr<TrackingMoveEnemyScript>> m_childList;
		// 生存時間
		int m_nExitTime;	
	protected:
		// ----- メソッド -----

		// 開始時に呼ばれます
		void Start() override;
		// 毎フレーム呼ばれます
		void Update() override;
		// 毎フレーム更新後に呼ばれます
		void LateUpdate() override;
		// 終了時に呼ばれます
		void End() override;

		// ----- コールバック関数 -----

		// 当たった時
		void OnDeltaCollisionEnter(DeltaCollider* collider) override;
		// 当たっている間
		void OnDeltaCollisionStay(DeltaCollider* collider) override;
	public:
		// 子の生成
		void CreateChild(int nNum);
	};
}
