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
#include "../Engine/ECSCompoent/Script.h"


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	class TrackingMoveEnemyScript : public Script
	{
	private:
		// ----- メンバ -----
		// リジッドボディ
		std::weak_ptr<Rigidbody> m_rb;

	public:
		// ターゲットのセット
		std::weak_ptr<Rigidbody> GetRb() { return m_rb.lock(); }

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
		void OnCollisionEnter(Collider* collider) override;
		// 当たっている間
		void OnCollisionStay (Collider* collider) override;
		// 離れた時
		void OnCollisionExit (Collider* collider) override;

		// ECS
		// 当たった時
		void OnECSCollisionEnter(SphereColliderData* collider) override;
		// 当たっている間
		void OnECSCollisionStay(SphereColliderData* collider) override;
		// 離れた時
		void OnECSCollisionExit(SphereColliderData* collider) override;
	};
}
