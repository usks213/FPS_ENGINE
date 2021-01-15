//==================================================================
//								GameOverScript.h
//	ゲームオーバー演出
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/15	ゲームオーバークラス作成
//
//===================================================================

#pragma once

//====== インクルード部 ======
#include "../Engine/ECSCompoent/Script.h"
#include "../Engine/ECSCompoent/SpriteRenderer.h"

//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	class GameOverScript : public Script
	{
	private:
		// ----- メンバ -----
		// スプライト
		std::weak_ptr<SpriteRenderer> m_sprite;
		// アルファ
		float m_fAlpha;
		float m_fAddAlpha;

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
