//==================================================================
//								FadeScript.h
//	フェードスクリプト
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/13	フェードスクリプトクラス作成
//
//===================================================================

#pragma once

//====== インクルード部 ======
#include "../Engine/ECSCompoent/Script.h"
#include <functional>
#include "../Engine/ECSCompoent/SpriteRenderer.h"

//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	class FadeScript : public Script
	{
	private:
		// ----- メンバ -----
		int m_nAlpha;	// フェード用ポリゴンのアルファ値(0～255)
		int m_nState;	// 状態
		int m_nFrame;	// フェードにかかるフレーム数
		bool m_bNowFade;// フェード中か

		std::function<void(void)> m_fadeEndFunc;	// フェード後の関数
		std::weak_ptr<SpriteRenderer> m_render;		// レンダラー
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

	public:
		// --- フェード

		// フェードアウトの開始
		void StartFadeOut(int nFrame = 60);
		// フェード中か
		bool GetIsFade() { return m_bNowFade; }
		// フェード後の関数オブジェクトを登録
		void SetFadeEndFunc(std::function<void(void)> func) { m_fadeEndFunc = func; }
	};
}
