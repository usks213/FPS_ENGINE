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
	m_bCurState(false), m_bOldState(false), m_bTriggr(false)
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
	Vector3 scale1 = collider->m_transform.lock()->m_scale;
	Vector3 scale2 = other->m_transform.lock()->m_scale;
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
	// 判定はAABBで取っている
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

	// 判定
	bool bCol = true;

	// 距離
	Vector3 maxLen = boxMax2 - boxMin1;
	Vector3	minLen = boxMax1 - boxMin2;


	//--- 押し出し
	// トリガーON
	if (collider->m_bTriggr || other->m_bTriggr) return bCol;

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

	//--- 最短距離軸を出す
	// X軸
	if (len->x <= len->y && len->x <= len->z)
	{
		// 押し出し
		collider->m_transform.lock()->m_pos->x = pos->x;
		// 法線
		normal->y = 0.0f;
		normal->z = 0.0f;
	}
	// Y軸
	else if (len->y <= len->x && len->y <= len->z)
	{
		// 押し出し
		collider->m_transform.lock()->m_pos->y = pos->y;
		// 法線
		normal->x = 0.0f;
		normal->z = 0.0f;
	}
	// Z軸
	else if (len->z <= len->x && len->z <= len->y)
	{
		// 押し出し
		collider->m_transform.lock()->m_pos->z = pos->z;
		// 法線
		normal->x = 0.0f;
		normal->y = 0.0f;
	}

	//--- 物理

	// リジッドボディ取得
	const auto& rb1 = collider->rigidbody().lock();
	const auto& rb2 = other->rigidbody().lock();
	if (!rb1 || !rb2) return bCol;
	// 物理をするか
	if (!rb1->m_bUsePhysics) return bCol;


	// ベクトルの大きさ
	float magnitude = rb1->m_velocity.magnitude();
	// 壁ずりベクトル
	Vector3 scratch = Vector3::WallScratchVector(rb1->m_velocity, normal) * magnitude;
	// 垂直ベクトル
	Vector3 vertical = Vector3::WallVerticalVector(rb1->m_velocity, normal) * magnitude;
	// 座標12ベクトル
	//Vector3 c = rb1->transform().lock()->m_pos - rb2->transform().lock()->m_pos;
	//c = Vector3::WallVerticalVector(c, normal) * c.magnitude();


	//--- 垂直方向(反発)
	float m = -rb2->m_fMass / (rb1->m_fMass + rb2->m_fMass);
	float e = (rb1->m_e * rb2->m_e);
	//float d = Vector3::Dot(rb2->m_velocity - vertical, c);
	Vector3 verticalVector = vertical * m * e;


	//--- 水平方向(摩擦)
	// 水平方向の力
	Vector3 horizontalVector;
	// 垂直抗力
	Vector3 N = vertical;
	// 静止摩擦
	float myu_s = rb1->m_fStaticFriction * rb2->m_fStaticFriction;
	// 最大静止摩擦力より大きいか
	if (scratch.magnitude() > myu_s * N.magnitude())
	{
		// 動摩擦で計算
		float myu_d = rb1->m_fDynamicFriction * rb2->m_fDynamicFriction;
		float F_d = myu_d * N.magnitude();
		if (F_d > 1.0f) F_d = 1.0f;
		// 水平方向の力
		horizontalVector = scratch - scratch * F_d;
	}


	//--- ベクトルの合成
	Vector3 force = horizontalVector + verticalVector;
	// 最終的な力
	rb1->m_force = force;

	return bCol;
}

//========================================
//
//	ボックスと球
//
//========================================
bool Collider::BoxToSphere(Collider* collider, Collider* other)
{
	// デバック
	return BoxToBox(collider, other);

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

	// 判定
	bool bCol = false;

	// 距離
	Vector3 maxLen = spherePos - boxMax;
	Vector3	minLen = boxMin - spherePos;

	//--- 判定
	if (maxLen->x < radius && minLen->x < radius &&
		maxLen->y < radius && minLen->y < radius &&
		maxLen->z < radius && minLen->z < radius)
	{
		bCol = true;
	}
	else
	{
		return false;
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
		pos->x = spherePos->x + radius + boxSize->x;
	}
	else
	{
		len->x = minLen->x;
		pos->x = spherePos->x - radius - boxSize->x;
	}
	// Y軸
	if (maxLen->y < minLen->y)
	{
		len->y = maxLen->y;
		pos->y = spherePos->y + radius + boxSize->y;
	}
	else
	{
		len->y = minLen->y;
		pos->y = spherePos->y - radius - boxSize->y;
	}
	// Z軸
	if (maxLen->z < minLen->z)
	{
		len->z = maxLen->z;
		pos->z = spherePos->z + radius + boxSize->z;
	}
	else
	{
		len->z = minLen->z;
		pos->z = spherePos->z - radius - boxSize->z;
	}

	//--- 最短距離軸を出す
	// X軸
	if (len->x <= len->y && len->x <= len->z)
	{
		// 押し出し
		collider->m_transform.lock()->m_pos->x = pos->x;
	}
	// Y軸
	else if (len->y <= len->x && len->y <= len->z)
	{
		// 押し出し
		collider->m_transform.lock()->m_pos->y = pos->y;
	}
	// Z軸
	else if (len->z <= len->x && len->z <= len->y)
	{
		// 押し出し
		collider->m_transform.lock()->m_pos->z = pos->z;
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
	// デバック
	return BoxToBox(collider, other);

	//--- 詳細判定
	// 中心座標
	Vector3 boxPos = other->m_transform.lock()->m_pos + other->m_bound.GetCenter();
	Vector3 spherePos = collider->m_transform.lock()->m_pos + collider->m_bound.GetCenter();
	// ボックスの最大最小
	Vector3 boxMax = boxPos + Vector3(other->m_bound.GetMax() * other->m_transform.lock()->m_scale);
	Vector3 boxMin = boxPos + Vector3(other->m_bound.GetMin() * other->m_transform.lock()->m_scale);
	// 半径
	float radius = collider->m_bound.GetRadius() * collider->m_transform.lock()->m_scale->x;

	// 判定
	bool bCol = false;

	// 距離
	Vector3 maxLen = spherePos - boxMax;
	Vector3	minLen = boxMin - spherePos;

	//--- 判定
	if (maxLen->x < radius && minLen->x < radius &&
		maxLen->y < radius && minLen->y < radius &&
		maxLen->z < radius && minLen->z < radius)
	{
		bCol = true;
	}
	else
	{
		return false;
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
		collider->m_transform.lock()->m_pos->x = pos->x;
	}
	// Y軸
	else if (len->y <= len->x && len->y <= len->z)
	{
		// 押し出し
		collider->m_transform.lock()->m_pos->y = pos->y;
	}
	// Z軸
	else if (len->z <= len->x && len->z <= len->y)
	{
		// 押し出し
		collider->m_transform.lock()->m_pos->z = pos->z;
	}

	//--- 物理
	return bCol;

	return false;
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
	// 衝突フラグ
	bool bCol = false;

	// 二点間距離
	Vector3 distance = pos1 - pos2;

	//--- 衝突判定
	if (distance.magnitudeNoSqrt() < (radius1 + radius2) * (radius1 + radius2))
	{
		bCol = true;
	}
	else
	{
		return false;
	}

	//---  押し出し
	// トリガー
	if (collider->m_bTriggr || other->m_bTriggr) return bCol;

	// 二点間と２半径の差
	float len = (radius1 + radius2) - distance.magnitude();
	// 押し出す方向
	Vector3 vec = distance.normalized() * len;
	// 押し出し
	collider->m_transform.lock()->m_pos += vec;


	//--- 物理
	return true;

}

