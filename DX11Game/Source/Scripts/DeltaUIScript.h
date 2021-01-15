//==================================================================
//								DeltaUIScript.h
//	Δ数表示UI
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/15	Δ表示UIクラス作成
//
//===================================================================

#pragma once

//====== インクルード部 ======
#include "../Engine/ECSCompoent/Script.h"
#include "PlayerScript.h"
#include "../Engine/ECSCompoent/SpriteRenderer.h"


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	class DeltaUIScript;

	class DeltaUIData :public Script
	{
		friend DeltaUIScript;
	private:
		// ----- メンバ -----
		std::weak_ptr<SpriteRenderer> m_sprite;
		int m_nAnimNo;

	public:
		// ----- メソッド -----

		// 開始時に呼ばれます
		void Start() override;

	};

	class DeltaUIScript : public Script
	{
	private:
		// ----- メンバ -----

		// プレイヤー
		std::weak_ptr<PlayerScript> m_player;

		// 数字
		std::vector<std::weak_ptr<DeltaUIData>> m_data;

	public:
		// プレイヤーの取得
		void SetPlayer(std::weak_ptr<PlayerScript> player) { m_player = player; }

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

		// UV更新
		void UpdateTexCoord();
	};
}
