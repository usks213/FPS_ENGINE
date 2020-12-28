//==================================================================
//												RigidbodySystem.h
//	リジッドボディ管理システム
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/27	リジッドボディシステム作成
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
	class Rigidbody;

	class RigidbodySystem final : public System<Rigidbody>
	{
	public:
		// コンストラクタ
		explicit RigidbodySystem(World* pWorld);
		// デストラクタ
		virtual ~RigidbodySystem();

		// 生成時
		void OnCreate() override;
		// 更新
		void OnUpdate() override;
		// 後更新
		void OnLateUpdate() override;
		// 削除時
		void OnDestroy() override;
	};
}
