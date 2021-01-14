//==================================================================
//								StartCrystalScript.h
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

#pragma once

//====== インクルード部 ======
#include "BombCrystalScript.h"


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	class StartCrystalScript : public BombCrystalScript
	{
	private:
		// ----- メンバ -----
		bool m_bStart;
		std::weak_ptr<GameObject> m_player;

	public:
		// プレイヤーのセット
		void SetPlayer(std::weak_ptr<GameObject> player) { m_player = player; }

	protected:
		// ----- メソッド -----

		// 開始時に呼ばれます
		void Start() override;
		// 毎フレーム更新後に呼ばれます
		void LateUpdate() override;
		// 終了時に呼ばれます
		void End() override;


		// ----- コールバック関数 -----

		// Delta
		// 当たった時
		void OnDeltaCollisionEnter(DeltaCollider* collider) override;
		// 当たっている間
		void OnDeltaCollisionStay(DeltaCollider* collider) override;
	};
}
