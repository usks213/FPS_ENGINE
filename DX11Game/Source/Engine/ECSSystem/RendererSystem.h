//==================================================================
//												RendererSystem.h
//	レンダラー管理システム
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/23	レンダラーシステム作成
//
//===================================================================

#pragma once

//====== インクルード部 ======
#include "../ECS/System/System.h"


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	// 前定義
	class Renderer;

	class RendererSystem final : public System<Renderer>
	{
	public:
		// コンストラクタ
		explicit RendererSystem(World* pWorld);
		// デストラクタ
		virtual ~RendererSystem();

		// 生成時
		void OnCreate() override;
		// 描画
		void OnDraw() override;
		// 削除時
		void OnDestroy() override;
	private:
		// ソート用
		static bool swapR(Renderer* lhs, Renderer* rhs);
	};
}
