//==================================================================
//												CollisionSystem.h
//	当たり判定管理システム
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/27	コリジョンシステム作成
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
	class Collider;

	class CollisionSystem final : public System<Collider>
	{
	public:
		// コンストラクタ
		explicit CollisionSystem(World* pWorld);
		// デストラクタ
		virtual ~CollisionSystem();

		// 生成時
		void OnCreate() override;
		// 更新
		void OnUpdate() override;
		// 削除時
		void OnDestroy() override;

	private:
		// ノードと空間の当たり判定
		static void Collision(Collider* main, const std::list<Collider*>& m_pSubList);
	};
}
