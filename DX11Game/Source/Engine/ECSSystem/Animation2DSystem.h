//==================================================================
//												Animation2DSystem.h
//	2Dアニメーション管理システム
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/23	アニメーション2Dシステム作成
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
	class Animator2D;

	class Animation2DSystem final : public System<Animator2D>
	{
	public:
		// コンストラクタ
		explicit Animation2DSystem(World* pWorld);
		// デストラクタ
		virtual ~Animation2DSystem();

		// 生成時
		void OnCreate() override;
		// 更新
		void OnUpdate() override;
		// 削除時
		void OnDestroy() override;
	};
}
