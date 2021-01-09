//==================================================================
//								TrackingMoveEnemyScript.h
//	追尾型エネミー
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/08	トラッキングムーブエネミークラス作成
//
//===================================================================

#pragma once

//====== インクルード部 ======
#include "EnemyBaseScript.h"


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	class TrackingMoveEnemyScript : public EnemyBaseScript
	{
	private:
		// ----- メンバ -----

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

	};
}
