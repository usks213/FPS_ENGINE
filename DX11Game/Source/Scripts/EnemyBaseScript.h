//==================================================================
//								EnemyBaseScript.h
//	エネミーベーススクリプト
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/09	エネミーベースクラス作成
//
//===================================================================

#pragma once

//====== インクルード部 ======
#include "../Engine/ECSCompoent/Script.h"


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	class MakeEnemyScript;

	class EnemyBaseScript : public Script
	{
		friend MakeEnemyScript;
	protected:
		// ----- メンバ -----
		// ターゲット
		std::weak_ptr<GameObject> m_player;
		// リジッドボディ
		std::weak_ptr<Rigidbody> m_rb;
		// 移動速度
		float m_speed;
	public:
		// リジッドボディ取得
		std::weak_ptr<Rigidbody> GetRb() { return m_rb; }
		// プレイヤーのセット
		void SetPlayer(std::weak_ptr<GameObject> player) { m_player = player; }
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

		// Delta
		// 当たった時
		virtual void OnDeltaCollisionEnter(DeltaCollider* collider) override;
		// 当たっている間
		virtual void OnDeltaCollisionStay(DeltaCollider* collider) override;
		// 離れた時
		virtual void OnDeltaCollisionExit(DeltaCollider* collider) override;
	};
}
