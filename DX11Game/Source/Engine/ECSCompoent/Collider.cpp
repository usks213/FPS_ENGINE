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

#include "../ECSEntity/GameObject.h"
#include "Transform.h"
#include "Rigidbody.h"

#include <math.h>

using namespace ECS;

//===== マクロ定義 =====



//===== プロトタイプ宣言 =====
// AABBと点の最短距離
float LenAABBToPoint(Bounds& box, Vector3& p);
Vector3 LenAABBToPoint(Vector3& min, Vector3& max, Vector3& p);



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
	:m_bound(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f)),
	m_bCurState(false), m_bOldState(false), m_bTriggr(false),
	m_eType(EColliderType::eBox)
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
	m_transform = m_Parent.lock()->GetComponent<Transform>();
	// ゲームオブジェクト取得
	m_gameObject = m_transform.lock()->gameObject();
	// リジッドボディ取得
	m_rigidbody = m_gameObject.lock()->GetComponent<Rigidbody>();
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
	Vector3 pos1 = collider->m_transform.lock()->m_pos + bound1.GetCenter();
	Vector3 pos2 = other->m_transform.lock()->m_pos + bound2.GetCenter();
	// スケール
	Vector3& scale1 = collider->m_transform.lock()->m_scale;
	Vector3& scale2 = other->m_transform.lock()->m_scale;
	// 最大
	Vector3 max1 = pos1 + bound1.GetMax() * scale1;
	Vector3 max2 = pos2 + bound2.GetMax() * scale2;
	// 最小
	Vector3 min1 = pos1 + bound1.GetMin() * scale1;
	Vector3 min2 = pos2 + bound2.GetMin() * scale2;

	// 衝突判定
	//if (min1->x > max2->x) return false;
	//if (max1->x < min2->x) return false;
	//if (min1->y > max2->y) return false;
	//if (max1->y < min2->y) return false;
	//if (min1->z > max2->z) return false;
	//if (max1->z < min2->z) return false;
	// return true;

	// 衝突判定
	if (min1->x < max2->x && min2->x < max1->x &&
		min1->y < max2->y && min2->y < max1->y &&
		min1->z < max2->z && min2->z < max1->z)
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
	//--- 判定はAABBで取っている
	// トリガーなし
	if (collider->m_bTriggr || other->m_bTriggr) return true;
	// リジッドボディ取得
	const auto& rb1 = collider->rigidbody().lock();
	const auto& rb2 = other->rigidbody().lock();
	if (!rb1 || !rb2) return true;


	//--- 詳細判定
	// 中心座標
	Vector3 boxPos1 = collider->m_transform.lock()->m_pos + collider->m_bound.GetCenter();
	Vector3 boxPos2 = other->m_transform.lock()->m_pos + other->m_bound.GetCenter();
	// ボックスの最大最小
	Vector3 boxMax1 = boxPos1 + Vector3(collider->m_bound.GetMax() * collider->m_transform.lock()->m_scale);
	Vector3 boxMin1 = boxPos1 + Vector3(collider->m_bound.GetMin() * collider->m_transform.lock()->m_scale);
	Vector3 boxMax2 = boxPos2 + Vector3(other->m_bound.GetMax() * other->m_transform.lock()->m_scale);
	Vector3 boxMin2 = boxPos2 + Vector3(other->m_bound.GetMin() * other->m_transform.lock()->m_scale);

	// ハーフサイズ
	Vector3 boxSize1 = collider->m_bound.GetHalfSize() * collider->m_transform.lock()->m_scale;
	Vector3 boxSize2 = other->m_bound.GetHalfSize() * other->m_transform.lock()->m_scale;


	//====================================================================
	// 距離
	Vector3 maxLen = boxMax2 - boxMin1;
	Vector3	minLen = boxMax1 - boxMin2;

	// 各軸距離
	Vector3 len;
	Vector3 pos;
	Vector3 normal;

	// X軸
	if (maxLen->x < minLen->x)
	{
		len->x = maxLen->x;
		pos->x = boxMax2->x + boxSize1->x;
		normal->x = 1.0f;
	}
	else
	{
		len->x = minLen->x;
		pos->x = boxMin2->x - boxSize1->x;
		normal->x = -1.0f;
	}
	// Y軸
	if (maxLen->y < minLen->y)
	{
		len->y = maxLen->y;
		pos->y = boxMax2->y + boxSize1->y;
		normal->y = 1.0f;
	}
	else
	{
		len->y = minLen->y;
		pos->y = boxMin2->y - boxSize1->y;
		normal->y = -1.0f;
	}
	// Z軸
	if (maxLen->z < minLen->z)
	{
		len->z = maxLen->z;
		pos->z = boxMax2->z + boxSize1->y;
		normal->z = 1.0f;
	}
	else
	{
		len->z = minLen->z;
		pos->z = boxMin2->z - boxSize1->y;
		normal->z = -1.0f;
	}

	// 符号
	len->x = fabsf(len->x);
	len->y = fabsf(len->y);
	len->z = fabsf(len->z);

	//--- 押し出し
	//--- 最短距離軸を出す
	// X軸
	if (len->x <= len->y && len->x <= len->z)
	{
		if (rb1->m_bUsePhysics)
		{
			// 押し出し
			collider->m_transform.lock()->m_pos->x = pos->x;
		}
		else
		{
			if (rb2->m_bUsePhysics)
			// 相手も
			other->m_transform.lock()->m_pos->x = boxPos1->x - (boxSize1->x + boxSize2->x) * normal->x;
		}
		// 法線
		normal->y = 0.0f;
		normal->z = 0.0f;
	}
	// Y軸
	else if (len->y <= len->x && len->y <= len->z)
	{
		if (rb1->m_bUsePhysics)
		{
			// 押し出し
			collider->m_transform.lock()->m_pos->y = pos->y; 
		}
		else
		if (rb2->m_bUsePhysics)
		{
			// 相手も
			other->m_transform.lock()->m_pos->y = boxPos1->y - (boxSize1->y + boxSize2->y) * normal->y;
		}
		// 法線
		normal->x = 0.0f;
		normal->z = 0.0f;
	}
	// Z軸
	else if (len->z <= len->x && len->z <= len->y)
	{
		if (rb1->m_bUsePhysics)
		{
			// 押し出し
			collider->m_transform.lock()->m_pos->z = pos->z;
		}
		else
		{
			if (rb2->m_bUsePhysics)
			// 相手も
			other->m_transform.lock()->m_pos->z = boxPos1->z - (boxSize1->z + boxSize2->z) * normal->z;
		}
		// 法線
		normal->x = 0.0f;
		normal->y = 0.0f;
	}

	////--- 物理
	//--- 物理
	// 物理をしない
	if (rb1->GetUsePhysics())
	{
		// 物理計算
		rb1->CollisionPhysics(rb2, normal);
	}
	else
	{
		// 物理計算
		rb2->CollisionPhysics(rb1, normal);
	}

	return true;
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
	Vector3 boxPos = collider->m_transform.lock()->m_pos + collider->m_bound.GetCenter();
	Vector3 spherePos = other->m_transform.lock()->m_pos + other->m_bound.GetCenter();
	// ボックスの最大最小
	Vector3 boxMax = boxPos + Vector3(collider->m_bound.GetMax() * collider->m_transform.lock()->m_scale);
	Vector3 boxMin = boxPos + Vector3(collider->m_bound.GetMin() * collider->m_transform.lock()->m_scale);
	// 半径
	float radius = other->m_bound.GetRadius() * other->m_transform.lock()->m_scale->x;
	// ハーフサイズ
	Vector3 boxSize = collider->m_bound.GetHalfSize() * collider->m_transform.lock()->m_scale;
	// 最短距離を計算
	Vector3 vlen = LenAABBToPoint(boxMin, boxMax, spherePos);
	float len = vlen.magnitude();

	//--- 判定
	// 当たってない
	if (len >= radius) return false;


	//--- 物理
	// トリガーなし
	if (collider->m_bTriggr || other->m_bTriggr) return true;
	// リジッドボディ取得
	const auto& rb1 = collider->rigidbody().lock();
	const auto& rb2 = other->rigidbody().lock();
	if (!rb1 || !rb2) return true;

	//// 物理をしない
	//if (!rb1->GetUsePhysics()) return true;
	//// 物理計算
	//rb1->CollisionPhysics(rb2, vlen.normalized());

	////--- 押し出し
	//// 二点間と２半径の差
	//len = radius - len;
	//// 押し出す方向
	//Vector3 vec = vlen.normalized() * len;
	//// 押し出し
	//collider->m_transform.lock()->m_pos -= vec;


	// 物理をしない
	if (rb1->GetUsePhysics())
	{
		// 物理計算
		rb1->CollisionPhysics(rb2, vlen.normalized());

		//--- 押し出し
		// 二点間と２半径の差
		len = radius - len;
		// 押し出す方向
		Vector3 vec = vlen.normalized() * len;
		// 押し出し
		collider->m_transform.lock()->m_pos -= vec;
	}
	else
	{
		if (rb2->GetUsePhysics())
		{
			// 物理計算
			rb2->CollisionPhysics(rb1, vlen.normalized());

			//--- 押し出し
			// 二点間と２半径の差
			len = radius - len;
			// 押し出す方向
			Vector3 vec = vlen.normalized() * len;
			// 押し出し
			other->m_transform.lock()->m_pos += vec;
		}
	}

	return true;
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
	Vector3 boxPos = other->m_transform.lock()->m_pos + other->m_bound.GetCenter();
	Vector3 spherePos = collider->m_transform.lock()->m_pos + collider->m_bound.GetCenter();
	// ボックスの最大最小
	Vector3 boxMax = boxPos + Vector3(other->m_bound.GetMax() * other->m_transform.lock()->m_scale);
	Vector3 boxMin = boxPos + Vector3(other->m_bound.GetMin() * other->m_transform.lock()->m_scale);
	// 半径
	float radius = collider->m_bound.GetRadius() * collider->m_transform.lock()->m_scale->x;
	// ハーフサイズ
	Vector3 boxSize = collider->m_bound.GetHalfSize() * collider->m_transform.lock()->m_scale;
	// 最短距離を計算
	Vector3 vlen = LenAABBToPoint(boxMin, boxMax, spherePos);
	float len = vlen.magnitude();

	//--- 判定
	// 当たってない
	if (len >= radius) return false;

	
	//--- 物理
	// トリガーなし
	if (collider->m_bTriggr || other->m_bTriggr) return true;
	// リジッドボディ取得
	const auto& rb1 = collider->rigidbody().lock();
	const auto& rb2 = other->rigidbody().lock();
	if (!rb1 || !rb2) return true;

	//// 物理をしない
	//if (!rb1->GetUsePhysics()) return true;
	//// 物理計算
	//rb1->CollisionPhysics(rb2, vlen.normalized());


	////--- 押し出し
	//// 二点間と２半径の差
	//len = radius - len;
	//// 押し出す方向
	//Vector3 vec = vlen.normalized() * len;
	//// 押し出し
	//collider->m_transform.lock()->m_pos += vec;


	// 物理をしない
	if (rb1->GetUsePhysics())
	{
		// 物理計算
		rb1->CollisionPhysics(rb2, vlen.normalized());

		//--- 押し出し
		// 二点間と２半径の差
		len = radius - len;
		// 押し出す方向
		Vector3 vec = vlen.normalized() * len;
		// 押し出し
		collider->m_transform.lock()->m_pos += vec;
	}
	else
	{
		if (rb2->GetUsePhysics())
		{
			// 物理計算
			rb2->CollisionPhysics(rb1, vlen.normalized());

			//--- 押し出し
			// 二点間と２半径の差
			len = radius - len;
			// 押し出す方向
			Vector3 vec = vlen.normalized() * len;
			// 押し出し
			other->m_transform.lock()->m_pos -= vec;
		}
	}

	return true;
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
	Vector3 pos1 = collider->m_transform.lock()->m_pos + collider->m_bound.GetCenter();
	Vector3 pos2 = other->m_transform.lock()->m_pos + other->m_bound.GetCenter();
	// 半径
	float radius1 = collider->m_bound.GetRadius() * collider->m_transform.lock()->m_scale->x;
	float radius2 = other->m_bound.GetRadius() * other->m_transform.lock()->m_scale->x;

	// 二点間距離
	Vector3 distance = pos1 - pos2;

	//--- 衝突判定
	if (distance.magnitudeNoSqrt() >= (radius1 + radius2) * (radius1 + radius2)) return false;

	
	//--- 物理
	// トリガー
	if (collider->m_bTriggr || other->m_bTriggr) return true;
	// リジッドボディ取得
	const auto& rb1 = collider->rigidbody().lock();
	const auto& rb2 = other->rigidbody().lock();
	if (!rb1 || !rb2) return true;

	// 物理をしない
	if (!rb1->GetUsePhysics()) return true;
	// 物理計算
	rb1->CollisionPhysics(rb2, distance.normalized());


	//---  押し出し
	// 二点間と２半径の差
	float len = (radius1 + radius2) - distance.magnitude();
	// 押し出す方向
	Vector3 vec = distance.normalized() * len;
	// 押し出し
	collider->m_transform.lock()->m_pos += vec;


	return true;

}

// AABBと点の最短距離
float LenAABBToPoint(Bounds& box, Vector3& p)
{
	// 長さのべき乗の値を格納
	float SqLen = 0;   
	
	// 各軸で点が最小値以下もしくは最大値以上ならば、差を考慮

	// X軸
	if (p->x < box.GetMin()->x)  
		SqLen += (p->x - box.GetMin()->x) * (p->x - box.GetMin()->x);
	if (p->x > box.GetMax()->x)
		SqLen += (p->x - box.GetMax()->x) * (p->x - box.GetMax()->x);
	
	// Y軸
	if (p->y < box.GetMin()->y)
		SqLen += (p->y - box.GetMin()->y) * (p->y - box.GetMin()->y);
	if (p->y > box.GetMax()->y)
		SqLen += (p->y - box.GetMax()->y) * (p->y - box.GetMax()->y);

	// Z軸
	if (p->z < box.GetMin()->z)
		SqLen += (p->z - box.GetMin()->z) * (p->z - box.GetMin()->z);
	if (p->z > box.GetMax()->z)
		SqLen += (p->z - box.GetMax()->z) * (p->z - box.GetMax()->z);

	// 平方根
	return sqrt(SqLen);
}

// AABBと点の最短距離
Vector3 LenAABBToPoint(Vector3& min, Vector3 &max, Vector3& p)
{
	// 長さのべき乗の値を格納
	Vector3 SqLen;

	// 各軸で点が最小値以下もしくは最大値以上ならば、差を考慮

	// X軸
	if (p->x < min->x)
		SqLen->x += (p->x - min->x);
	if (p->x > max->x)
		SqLen->x += (p->x - max->x);

	// Y軸
	if (p->y < min->y)
		SqLen->y += (p->y - min->y);
	if (p->y > max->y)
		SqLen->y += (p->y - max->y);

	// Z軸
	if (p->z < min->z)
		SqLen->z += (p->z - min->z);
	if (p->z > max->z)
		SqLen->z += (p->z - max->z);

	return SqLen;
}