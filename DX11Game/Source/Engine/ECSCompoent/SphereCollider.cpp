//==================================================================
//								SphereCollider.cpp
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


//====== インクルード部 ======
#include "SphereCollider.h"
#include "../ECSSystem/CollisionSystem.h"

#include "../ECS/Entity/EntityManager.h"

using namespace ECS;

//===== マクロ定義 =====


//========================================
//
//	コンストラクタ
//
//========================================
SphereCollider::SphereCollider()
{
	m_eType = EColliderType::eSphere;
}

//========================================
//
//	デストラクタ
//
//========================================
SphereCollider::~SphereCollider()
{
}

//========================================
//
//	衝突判定
//
//========================================
bool SphereCollider::Judgment(Collider* other)
{
	// 相手のコライダーで分岐
	switch (other->GetColliderType())
	{
	case EColliderType::eBox:
		return SphereToBox(this, other);
		break;
	case EColliderType::eSphere:
		return SphereToSphere(this, other);
		break;
	default:
		break;
	}

	return false;
}