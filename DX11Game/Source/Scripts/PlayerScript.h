//==================================================================
//								PlayerScript.h
//	プレイヤースクリプト
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/27	プレイヤースクリプトクラス作成(Test)
//	2021/01/09	デルタカウンターの追加
//
//===================================================================

#pragma once

//====== インクルード部 ======
#include "../Engine/ECSCompoent/Script.h"


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	class PlayerScript : public Script
	{
	private:
		// ----- メンバ -----
		std::weak_ptr<Rigidbody> m_rb;
		int m_nShotCnt;
		// ジャンプフラグ
		int m_nJump;
		// デルタカウンター
		int m_nDeltaCount;
		// ショット
		bool m_bShot;
		// 地面
		bool m_bGround;

		// ステータス
		float m_fHP;
		const float m_fMaxHP = 100.0f;

		// ヒール
		const int m_nHeelInteral = 480;
		int	  m_nHeelCnt;
		float m_fHeel;

		// ダメージ
		const int m_nDamageInteral = 300;
		int	  m_nDamageCnt;
		float m_fDamage;



	public:
		// デルタ数取得
		int GetDeltaCount() { return m_nDeltaCount; }

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
		// 離れた時
		void OnDeltaCollisionExit(DeltaCollider* collider) override;
	};
}
