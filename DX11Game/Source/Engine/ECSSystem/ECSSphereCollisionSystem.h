//==================================================================
//												ECSSphereCollisionSystem.h
//	データ指向版球当たり判定管理システム
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/01/06	データ指向版で作成
//
//===================================================================

#pragma once

//====== インクルード部 ======
#include "../ECS/System/ECSSystem.h"


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	// 前定義
	class SphereColliderData;

	class ECSSphereCollisionSystem final : public ECSSystem<SphereColliderData>
	{
	public:
		// コンストラクタ
		explicit ECSSphereCollisionSystem(World* pWorld);
		// デストラクタ
		virtual ~ECSSphereCollisionSystem();

		// 生成時
		void OnCreate() override;
		// 更新
		void OnUpdate() override;
		// 削除時
		void OnDestroy() override;

	private:
		// 当たり判定
		static void Collision(SphereColliderData* main, const std::list<SphereColliderData*>& m_pSubList);
	};
}
