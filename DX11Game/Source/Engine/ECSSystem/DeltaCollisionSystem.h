//==================================================================
//												DeltaCollisionSystem.h
//	デルタ専用当たり判定管理システム
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/09	デルタコリジョンシステム作成
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
	class DeltaCollider;

	class DeltaCollisionSystem final : public System<DeltaCollider>
	{
	public:
		// コンストラクタ
		explicit DeltaCollisionSystem(World* pWorld);
		// デストラクタ
		virtual ~DeltaCollisionSystem();

		// 生成時
		void OnCreate() override;
		// 更新
		void OnUpdate() override;
		// 削除時
		void OnDestroy() override;

	private:
		// ノードと空間の当たり判定
		static void Collision(DeltaCollider* main, const std::list<DeltaCollider*>& m_pSubList);
	};
}
