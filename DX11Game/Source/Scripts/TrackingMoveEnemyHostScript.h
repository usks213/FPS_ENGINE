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
#include "../Engine/ECSCompoent/Script.h"

#include "../Engine/ECSCompoent/ECSRigidbody.h"
#include "TrackingMoveEnemyScript.h"

//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	class TrackingMoveEnemyHostScript : public Script
	{
	private:
		// ----- メンバ -----

		// ターゲット
		std::weak_ptr<Transform> m_target;
		// リジッドボディ
		std::weak_ptr<ECSRigidbody> m_rb;
		// 移動速度
		float m_speed;

		// 子リスト
		std::list<std::weak_ptr<TrackingMoveEnemyScript>> m_childList;

	public:
		// ターゲットのセット
		void SetTarget(std::weak_ptr<Transform> target) { m_target = target; }
		// 子の生成
		void CreateChild(int nNum);

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
