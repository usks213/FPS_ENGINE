//==================================================================
//								BombCrystalScript.h
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

#pragma once

//====== インクルード部 ======
#include "../Engine/ECSCompoent/Script.h"


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	class BombCrystalScript : public Script
	{
	protected:
		// ----- メンバ -----
		int m_nExitTime;	// 生存時間

	protected:
		// ----- メソッド -----

		// 開始時に呼ばれます
		virtual void Start() override;
		// 毎フレーム呼ばれます
		void Update() override;
		// 毎フレーム更新後に呼ばれます
		virtual void LateUpdate() override;
		// 終了時に呼ばれます
		virtual void End() override;


		// ----- コールバック関数 -----

		// Delta
		// 当たった時
		virtual void OnDeltaCollisionEnter(DeltaCollider* collider) override;
		// 当たっている間
		virtual void OnDeltaCollisionStay(DeltaCollider* collider) override;
		// 離れた時
		void OnDeltaCollisionExit(DeltaCollider* collider) override;
	};
}
