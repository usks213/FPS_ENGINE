//==================================================================
//								TitleScoreUIScript.h
//	タイトルでのΔ数表示UI
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/15	タイトルΔ表示UIクラス作成
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
	class TitleScoreUIScript;

	class TitleScoreUIData :public Script
	{
		friend TitleScoreUIScript;
	private:
		// ----- メンバ -----
		std::weak_ptr<SpriteRenderer> m_sprite;
		int m_nAnimNo;

	public:
		// ----- メソッド -----

		// 開始時に呼ばれます
		void Start() override;

	};

	class TitleScoreUIScript : public Script
	{
	private:
		// ----- メンバ -----

		int m_nScore;

		// 数字
		std::vector<std::weak_ptr<TitleScoreUIData>> m_data;

	public:
		// スコアのセット
		void SetScore(int nScore) { m_nScore = nScore; }

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
