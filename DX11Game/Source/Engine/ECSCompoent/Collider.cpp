//==================================================================
//								Collider.h
//	コライダーベース
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/24	コライダーベースクラス作成
//
//===================================================================


//====== インクルード部 ======
#include "Collider.h"
#include "../ECSSystem/CollisionSystem.h"

#include "../ECS/Entity/EntityManager.h"

using namespace ECS;

//===== マクロ定義 =====



//***************** Bounds ******************

//========================================
//
//	コンストラクタ
//
//========================================
Bounds::Bounds(Vector3 center, Vector3 size)
	:m_center(center), m_size(size) 
{
}


//***************** Collider ******************


//========================================
//
//	コンストラクタ
//
//========================================
Collider::Collider()
	:m_bound(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f))
{


}

//========================================
//
//	デストラクタ
//
//========================================
Collider::~Collider()
{

}

//========================================
//
//	生成時
//
//========================================
void Collider::OnCreate()
{
	// システムに格納
	CollisionSystem* sys = GetEntityManager()->GetWorld()->GetSystem<CollisionSystem>();
	if (sys) sys->AddList(this);

	// トランスフォーム取得
	m_trans = m_Parent.lock()->GetComponent<Transform>();
}

//========================================
//
//	破棄時
//
//========================================
void Collider::OnDestroy()
{
	// システムから除外
	CollisionSystem* sys = GetEntityManager()->GetWorld()->GetSystem<CollisionSystem>();
	if (sys) sys->RemoveList(this);
}

//========================================
//
//	メッセージ受信
//
//========================================
void Collider::SendComponentMessage(const std::string& message, void* value)
{
	// Updateメッセージでマトリックスを更新
	if ("Update" == message)
	{
	}
}

//========================================
//
//	AABBtoAABB 仮判定用
//
//========================================
bool Collider::AABBtoAABB(Collider* collider, Collider* other)
{
	// バウンド
	Bounds& bound1 = collider->m_bound;
	Bounds& bound2 = other->m_bound;

	// 座標
	Vector3 pos1 = collider->m_trans.lock()->m_pos + bound1.GetCenter();
	Vector3 pos2 = other->m_trans.lock()->m_pos + bound2.GetCenter();
	// スケール
	Vector3 scale1 = collider->m_trans.lock()->m_scale;
	Vector3 scale2 = other->m_trans.lock()->m_scale;
	// 最大
	Vector3 max1 = pos1 + Vector3(bound1.GetMax() * scale1);
	Vector3 max2 = pos2 + Vector3(bound2.GetMax() * scale2);
	// 最小
	Vector3 min1 = pos1 + Vector3(bound1.GetMin() * scale1);
	Vector3 min2 = pos2 + Vector3(bound2.GetMin() * scale2);

	// 衝突判定
	//if (min1->x > max2->x) return false;
	//if (max1->x < min2->x) return false;
	//if (min1->y > max2->y) return false;
	//if (max1->y < min2->y) return false;
	//if (min1->z > max2->z) return false;
	//if (max1->z < min2->z) return false;

	// 衝突判定
	if (min1->x <= max2->x && min2->x <= max1->x &&
		min1->y <= max2->y && min2->y <= max1->y &&
		min1->z <= max2->z && min2->z <= max1->z)
	{
		return true;
	}

	// 非衝突
	return false;
}

//========================================
//
//	ボックスとボックス
//
//========================================
bool Collider::BoxToBox(Collider* collider, Collider* other)
{
	// 判定はAABBで取っている
	//--- 詳細判定
	// 中心座標
	Vector3 boxPos1 = collider->m_trans.lock()->m_pos + collider->m_bound.GetCenter();
	Vector3 boxPos2 = other->m_trans.lock()->m_pos + other->m_bound.GetCenter();
	// ボックスの最大最小
	Vector3 boxMax1 = boxPos1 + Vector3(collider->m_bound.GetMax() * collider->m_trans.lock()->m_scale);
	Vector3 boxMin1 = boxPos1 + Vector3(collider->m_bound.GetMin() * collider->m_trans.lock()->m_scale);
	Vector3 boxMax2 = boxPos2 + Vector3(other->m_bound.GetMax() * other->m_trans.lock()->m_scale);
	Vector3 boxMin2 = boxPos2 + Vector3(other->m_bound.GetMin() * other->m_trans.lock()->m_scale);

	// ハーフサイズ
	Vector3 boxSize2 = other->m_bound.GetHalfSize() * other->m_trans.lock()->m_scale;

	// 判定
	bool bCol = true;

	// 距離
	Vector3 maxLen = boxPos2 - boxMax1;
	Vector3	minLen = boxMin1 - boxPos2;


	//--- 押し出し
	// トリガーON
	if (collider->m_bTriggr || other->m_bTriggr) return bCol;

	// 各軸距離
	Vector3 len;
	Vector3 pos;

	// X軸
	if (maxLen->x < minLen->x)
	{
		len->x = maxLen->x;
		pos->x = boxMax1->x + boxSize2->x;
	}
	else
	{
		len->x = minLen->x;
		pos->x = boxMin1->x - boxSize2->x;
	}
	// Y軸
	if (maxLen->y < minLen->y)
	{
		len->y = maxLen->y;
		pos->y = boxMax1->y + boxSize2->y;
	}
	else
	{
		len->y = minLen->y;
		pos->y = boxMin1->y - boxSize2->y;
	}
	// Z軸
	if (maxLen->z < minLen->z)
	{
		len->z = maxLen->z;
		pos->z = boxMax1->z + boxSize2->y;
	}
	else
	{
		len->z = minLen->z;
		pos->z = boxMin1->z - boxSize2->y;
	}

	//--- 最短距離軸を出す
	// X軸
	if (len->x <= len->y && len->x <= len->z)
	{
		// 押し出し
		other->m_trans.lock()->m_pos->x = pos->x;
	}
	// Y軸
	else if (len->y <= len->x && len->y <= len->z)
	{
		// 押し出し
		other->m_trans.lock()->m_pos->y = pos->y;
	}
	// Z軸
	else if (len->z <= len->x && len->z <= len->y)
	{
		// 押し出し
		other->m_trans.lock()->m_pos->z = pos->z;
	}

	//--- 物理
	return bCol;
}

//========================================
//
//	ボックスと球
//
//========================================
bool Collider::BoxToSphere(Collider* collider, Collider* other)
{
	//--- 詳細判定
	// 中心座標
	Vector3 boxPos = collider->m_trans.lock()->m_pos + collider->m_bound.GetCenter();
	Vector3 spherePos = other->m_trans.lock()->m_pos + other->m_bound.GetCenter();
	// ボックスの最大最小
	Vector3 boxMax = boxPos + Vector3(collider->m_bound.GetMax() * collider->m_trans.lock()->m_scale);
	Vector3 boxMin = boxPos + Vector3(collider->m_bound.GetMin() * collider->m_trans.lock()->m_scale);
	// 半径
	float radius = other->m_bound.GetRadius() * other->m_trans.lock()->m_scale->x;

	// 判定
	bool bCol = false;

	// 距離
	Vector3 maxLen = spherePos - boxMax;
	Vector3	minLen = boxMin - spherePos;

	//--- 判定
	if (maxLen->x <= radius && minLen->x <= radius &&
		maxLen->y <= radius && minLen->y <= radius &&
		maxLen->z <= radius && minLen->z <= radius)
	{
		bCol = true;
	}


	//--- 押し出し
	// トリガーON
	if (collider->m_bTriggr || other->m_bTriggr) return bCol;

	// 各軸距離
	Vector3 len;
	Vector3 pos;

	// X軸
	if (maxLen->x < minLen->x)
	{
		len->x = maxLen->x;
		pos->x = boxMax->x + radius;
	}
	else
	{
		len->x = minLen->x;
		pos->x = boxMin->x - radius;
	}
	// Y軸
	if (maxLen->y < minLen->y)
	{
		len->y = maxLen->y;
		pos->y = boxMax->y + radius;
	}
	else
	{
		len->y = minLen->y;
		pos->y = boxMin->y - radius;
	}
	// Z軸
	if (maxLen->z < minLen->z)
	{
		len->z = maxLen->z;
		pos->z = boxMax->z + radius;
	}
	else
	{
		len->z = minLen->z;
		pos->z = boxMin->z - radius;
	}

	//--- 最短距離軸を出す
	// X軸
	if (len->x <= len->y && len->x <= len->z)
	{
		// 押し出し
		other->m_trans.lock()->m_pos->x = pos->x;
	}
	// Y軸
	else if (len->y <= len->x && len->y <= len->z)
	{
		// 押し出し
		other->m_trans.lock()->m_pos->y = pos->y;
	}
	// Z軸
	else if (len->z <= len->x && len->z <= len->y)
	{
		// 押し出し
		other->m_trans.lock()->m_pos->z = pos->z;
	}

	//--- 物理
	return bCol;

}

//========================================
//
//	球とボックス
//
//========================================
bool Collider::SphereToBox(Collider* collider, Collider* other)
{
	//--- 詳細判定
	// 中心座標
	Vector3 boxPos = other->m_trans.lock()->m_pos + other->m_bound.GetCenter();
	Vector3 spherePos = collider->m_trans.lock()->m_pos + collider->m_bound.GetCenter();
	// ボックスの最大最小
	Vector3 boxMax = boxPos + Vector3(other->m_bound.GetMax() * other->m_trans.lock()->m_scale);
	Vector3 boxMin = boxPos + Vector3(other->m_bound.GetMin() * other->m_trans.lock()->m_scale);
	// 半径
	float radius = collider->m_bound.GetRadius() * collider->m_trans.lock()->m_scale->x;

	// 判定
	bool bCol = false;

	// 距離
	Vector3 maxLen = spherePos - boxMax;
	Vector3	minLen = boxMin - spherePos;

	//--- 判定
	if (maxLen->x <= radius && minLen->x <= radius &&
		maxLen->y <= radius && minLen->y <= radius &&
		maxLen->z <= radius && minLen->z <= radius)
	{
		bCol = true;
	}


	//--- 押し出し
	// トリガーON
	if (collider->m_bTriggr || other->m_bTriggr) return bCol;

	// 各軸距離
	Vector3 len;
	Vector3 pos;

	// X軸
	if (maxLen->x < minLen->x)
	{
		len->x = maxLen->x;
		pos->x = boxMax->x + radius;
	}
	else
	{
		len->x = minLen->x;
		pos->x = boxMin->x - radius;
	}
	// Y軸
	if (maxLen->y < minLen->y)
	{
		len->y = maxLen->y;
		pos->y = boxMax->y + radius;
	}
	else
	{
		len->y = minLen->y;
		pos->y = boxMin->y - radius;
	}
	// Z軸
	if (maxLen->z < minLen->z)
	{
		len->z = maxLen->z;
		pos->z = boxMax->z + radius;
	}
	else
	{
		len->z = minLen->z;
		pos->z = boxMin->z - radius;
	}

	//--- 最短距離軸を出す
	// X軸
	if (len->x <= len->y && len->x <= len->z)
	{
		// 押し出し
		collider->m_trans.lock()->m_pos->x = pos->x;
	}
	// Y軸
	else if (len->y <= len->x && len->y <= len->z)
	{
		// 押し出し
		collider->m_trans.lock()->m_pos->y = pos->y;
	}
	// Z軸
	else if (len->z <= len->x && len->z <= len->y)
	{
		// 押し出し
		collider->m_trans.lock()->m_pos->z = pos->z;
	}

	//--- 物理
	return bCol;
}

//========================================
//
//	球と球
//
//========================================
bool Collider::SphereToSphere(Collider* collider, Collider* other)
{
	//--- 詳細判定
	// 中心座標
	Vector3 pos1 = collider->m_trans.lock()->m_pos + collider->m_bound.GetCenter();
	Vector3 pos2 = other->m_trans.lock()->m_pos + other->m_bound.GetCenter();
	// 半径
	float radius1 = collider->m_bound.GetRadius() * collider->m_trans.lock()->m_scale->x;
	float radius2 = other->m_bound.GetRadius() * other->m_trans.lock()->m_scale->x;
	// 衝突フラグ
	bool bCol = false;

	// 二点間距離
	Vector3 distance = pos1 - pos2;

	//--- 衝突判定
	if (distance.magnitudeNoSqrt() < (radius1 + radius2) + (radius1 + radius2))
	{
		bCol = true;
	}

	//---  押し出し
	// トリガー
	if (collider->m_bTriggr || other->m_bTriggr) return bCol;

	// 二点間と２半径の差
	float len = (radius1 + radius2) - distance.magnitude();
	// 押し出す方向
	Vector3 vec = distance.normalized() * len;
	// 押し出し
	collider->m_trans.lock()->m_pos += vec;


	//--- 物理
	return true;

}

