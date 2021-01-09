//==================================================================
//								DeltaDeltaCollider.cpp
//	デルタ専用コライダー
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/09	デルタ専用コライダーベースクラス作成
//
//===================================================================


//====== インクルード部 ======
#include "DeltaCollider.h"
#include "../ECSSystem/DeltaCollisionSystem.h"

#include "../ECS/Entity/EntityManager.h"

#include "../ECSEntity/GameObject.h"
#include "Transform.h"
#include "Rigidbody.h"

#include <math.h>

using namespace ECS;

//===== マクロ定義 =====



//===== プロトタイプ宣言 =====


//***************** DeltaCollider ******************


//========================================
//
//	コンストラクタ
//
//========================================
DeltaCollider::DeltaCollider()
	:m_bound(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f)),
	m_bCurState(false), m_bOldState(false), m_bMain(true)
{
}

//========================================
//
//	デストラクタ
//
//========================================
DeltaCollider::~DeltaCollider()
{
}

//========================================
//
//	生成時
//
//========================================
void DeltaCollider::OnCreate()
{
	// システムに格納
	DeltaCollisionSystem* sys = GetEntityManager()->GetWorld()->GetSystem<DeltaCollisionSystem>();
	if (sys) sys->AddList(this);

	// トランスフォーム取得
	m_transform = m_Parent.lock()->GetComponent<Transform>();
	// ゲームオブジェクト取得
	m_gameObject = m_transform.lock()->gameObject();
}

//========================================
//
//	破棄時
//
//========================================
void DeltaCollider::OnDestroy()
{
	// システムから除外
	DeltaCollisionSystem* sys = GetEntityManager()->GetWorld()->GetSystem<DeltaCollisionSystem>();
	if (sys) sys->RemoveList(this);
}

//========================================
//
//	メッセージ受信
//
//========================================
void DeltaCollider::SendComponentMessage(const std::string& message, void* value)
{
}

//========================================
//
// 球と球の当たり判定
//
//========================================
bool DeltaCollider::SpheretoSphere(DeltaCollider* collider, DeltaCollider* other)
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
	return true;
}