//==================================================================
//									Animator2D.h
//	アニメーター2D
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/02	アニメーター2Dクラス作成
//
//===================================================================
#pragma once

//====== インクルード部 ======
#include "../ECS/Component/IComponent.h"
#include <map>


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	// 前定義
	class Animation2D;
	class Renderer;

	// メモ： 汎用型にして
	class Animator2D final : public IComponent
	{
	public:
		// コンストラクタ
		Animator2D();
		// デストラクタ
		~Animator2D();

		// オブジェクト生成時
		void OnCreate() override;
		// オブジェクト破棄時
		void OnDestroy() override;

		// アニメーションアップデート
		void AnimationUpdate();

		// アニメーションリスト管理
		void AddAnimationSelect(const std::string name, int nMaxFrame, ...);
		void AddAnimationDefault(const std::string name, int nMaxFrame);
		void SetAnimation(const std::string name);
		void SetAnimation(const std::string name, int nSpeed);
		void SetAnimation(const std::string name, int nSpeed, int nSplitX, int nSplitY);

		// セット関数
		void SetSplit(int nSplitX, int nSplitY);
		void SetAnimationSpeed(int nSpeed) { m_nSpeed = nSpeed; }

		// ゲット関数
		std::weak_ptr<Animation2D> GetCurrentAnimation() { return m_pCurrentAnimation; }

	private:
		// プール
		std::map<const std::string, std::shared_ptr<Animation2D>> m_animationPool;

		// レンダラー
		std::weak_ptr<Renderer> m_renderer;

		// アニメーションの変数
		int m_nSpeed;
		int m_nSplitX;
		int m_nSplitY;
		int m_nFrame;
		int m_nCurrentAnimNo;
		std::weak_ptr<Animation2D> m_pCurrentAnimation;
	};
}