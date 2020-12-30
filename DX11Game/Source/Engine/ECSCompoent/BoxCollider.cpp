//==================================================================
//								BoxCollider.cpp
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


//====== インクルード部 ======
#include "BoxCollider.h"
#include "../ECSSystem/CollisionSystem.h"

#include "../ECS/Entity/EntityManager.h"

using namespace ECS;

//===== マクロ定義 =====


//========================================
//
//	コンストラクタ
//
//========================================
BoxCollider::BoxCollider()
{
	m_eType = EColliderType::eBox;
}

//========================================
//
//	デストラクタ
//
//========================================
BoxCollider::~BoxCollider()
{
}

//========================================
//
//	衝突判定
//
//========================================
bool BoxCollider::Judgment(Collider* other)
{
	// 相手のコライダーで分岐
	switch (other->GetColliderType())
	{
	case EColliderType::eBox:
		return BoxToBox(this, other);
		break;
	case EColliderType::eSphere:
		return BoxToSphere(this, other);
		break;
	default:
		break;
	}

	return false;
}