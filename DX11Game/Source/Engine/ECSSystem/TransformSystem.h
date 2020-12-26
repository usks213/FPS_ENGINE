//==================================================================
//												TransformSystem.h
//	トランスフォーム管理システム
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/23	トランスフォームシステム作成
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
	class Transform;

	class TransformSystem final : public System<Transform>
	{
	public:
		// コンストラクタ
		explicit TransformSystem(World* pWorld);
		// デストラクタ
		virtual ~TransformSystem();

		// 生成時
		void OnCreate() override;
		// 更新
		void OnUpdate() override;
		// 削除時
		void OnDestroy() override;
	};
}
