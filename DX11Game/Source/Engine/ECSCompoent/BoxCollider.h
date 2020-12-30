//==================================================================
//								BoxCollider.h
//	ボックスコライダー
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/27	ボックスコライダークラス作成
//
//===================================================================

#pragma once

//====== インクルード部 ======
#include "Collider.h"

//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	class BoxCollider final : public Collider
	{
	
	public:
		// コンストラクタ
		BoxCollider();
		// デストラクタ
		~BoxCollider();

		// 当たり判定サイズ
		void SetSize(Vector3 size) { m_bound.SetSize(size); }

	protected:
		// 各コライダーの当たり判定
		bool Judgment(Collider* other) override;
	};
}
