//==================================================================
//								SphereCollider.h
//	スフィアコライダー
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/27	スフィアコライダークラス作成
//
//===================================================================

#pragma once

//====== インクルード部 ======
#include "Collider.h"

//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	class SphereCollider final : public Collider
	{
	
	public:
		// コンストラクタ
		SphereCollider();
		// デストラクタ
		~SphereCollider();

		// 当たり判定サイズ
		void SetRadius(float radius) { m_bound.SetSize(Vector3(2,2,2) * radius); }

	protected:
		// 各コライダーの当たり判定
		bool Judgment(Collider* other) override;
	};
}
