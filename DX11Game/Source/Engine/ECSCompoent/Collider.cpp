//==================================================================
//								Collider.h
//	RC_[x[X
//
//==================================================================
//	author :	AT12A 05 F²όWV
//==================================================================
//	J­π
//
//	2020/12/24	RC_[x[XNXμ¬
//
//===================================================================


//====== CN[h ======
#include "Collider.h"
#include "../ECSSystem/CollisionSystem.h"

#include "../ECS/Entity/EntityManager.h"

using namespace ECS;

//===== }Nθ` =====



//***************** Bounds ******************

//========================================
//
//	RXgN^
//
//========================================
Bounds::Bounds(Vector3 center, Vector3 size)
	:m_center(center), m_size(size) 
{
}


//***************** Collider ******************


//========================================
//
//	RXgN^
//
//========================================
Collider::Collider()
	:m_bound(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f))
{


}

//========================================
//
//	fXgN^
//
//========================================
Collider::~Collider()
{

}

//========================================
//
//	Ά¬
//
//========================================
void Collider::OnCreate()
{
	// VXeΙi[
	CollisionSystem* sys = GetEntityManager()->GetWorld()->GetSystem<CollisionSystem>();
	if (sys) sys->AddList(this);

	// gXtH[ζΎ
	m_trans = m_Parent.lock()->GetComponent<Transform>();
}

//========================================
//
//	jό
//
//========================================
void Collider::OnDestroy()
{
	// VXe©ηO
	CollisionSystem* sys = GetEntityManager()->GetWorld()->GetSystem<CollisionSystem>();
	if (sys) sys->RemoveList(this);
}

//========================================
//
//	bZ[WσM
//
//========================================
void Collider::SendComponentMessage(const std::string& message, void* value)
{
	// UpdatebZ[WΕ}gbNXπXV
	if ("Update" == message)
	{
	}
}

//========================================
//
//	AABBtoAABB Ό»θp
//
//========================================
bool Collider::AABBtoAABB(Collider* collider, Collider* other)
{
	// oEh
	Bounds& bound1 = collider->m_bound;
	Bounds& bound2 = other->m_bound;

	// ΐW
	Vector3 pos1 = collider->m_trans.lock()->m_pos + bound1.GetCenter();
	Vector3 pos2 = other->m_trans.lock()->m_pos + bound2.GetCenter();
	// XP[
	Vector3 scale1 = collider->m_trans.lock()->m_scale;
	Vector3 scale2 = other->m_trans.lock()->m_scale;
	// Εε
	Vector3 max1 = pos1 + Vector3(bound1.GetMax() * scale1);
	Vector3 max2 = pos2 + Vector3(bound2.GetMax() * scale2);
	// Ε¬
	Vector3 min1 = pos1 + Vector3(bound1.GetMin() * scale1);
	Vector3 min2 = pos2 + Vector3(bound2.GetMin() * scale2);

	// ΥΛ»θ
	//if (min1->x > max2->x) return false;
	//if (max1->x < min2->x) return false;
	//if (min1->y > max2->y) return false;
	//if (max1->y < min2->y) return false;
	//if (min1->z > max2->z) return false;
	//if (max1->z < min2->z) return false;

	// ΥΛ»θ
	if (min1->x <= max2->x && min2->x <= max1->x &&
		min1->y <= max2->y && min2->y <= max1->y &&
		min1->z <= max2->z && min2->z <= max1->z)
	{
		return true;
	}

	// ρΥΛ
	return false;
}

//========================================
//
//	{bNXΖ{bNX
//
//========================================
bool Collider::BoxToBox(Collider* collider, Collider* other)
{
	// »θΝAABBΕζΑΔ’ι
	//--- ΪΧ»θ
	// SΐW
	Vector3 boxPos1 = collider->m_trans.lock()->m_pos + collider->m_bound.GetCenter();
	Vector3 boxPos2 = other->m_trans.lock()->m_pos + other->m_bound.GetCenter();
	// {bNXΜΕεΕ¬
	Vector3 boxMax1 = boxPos1 + Vector3(collider->m_bound.GetMax() * collider->m_trans.lock()->m_scale);
	Vector3 boxMin1 = boxPos1 + Vector3(collider->m_bound.GetMin() * collider->m_trans.lock()->m_scale);
	Vector3 boxMax2 = boxPos2 + Vector3(other->m_bound.GetMax() * other->m_trans.lock()->m_scale);
	Vector3 boxMin2 = boxPos2 + Vector3(other->m_bound.GetMin() * other->m_trans.lock()->m_scale);

	// n[tTCY
	Vector3 boxSize2 = other->m_bound.GetHalfSize() * other->m_trans.lock()->m_scale;

	// »θ
	bool bCol = true;

	// £
	Vector3 maxLen = boxPos2 - boxMax1;
	Vector3	minLen = boxMin1 - boxPos2;


	//--- ΅o΅
	// gK[ON
	if (collider->m_bTriggr || other->m_bTriggr) return bCol;

	// e²£
	Vector3 len;
	Vector3 pos;

	// X²
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
	// Y²
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
	// Z²
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

	//--- ΕZ£²πo·
	// X²
	if (len->x <= len->y && len->x <= len->z)
	{
		// ΅o΅
		other->m_trans.lock()->m_pos->x = pos->x;
	}
	// Y²
	else if (len->y <= len->x && len->y <= len->z)
	{
		// ΅o΅
		other->m_trans.lock()->m_pos->y = pos->y;
	}
	// Z²
	else if (len->z <= len->x && len->z <= len->y)
	{
		// ΅o΅
		other->m_trans.lock()->m_pos->z = pos->z;
	}

	//--- ¨
	return bCol;
}

//========================================
//
//	{bNXΖ
//
//========================================
bool Collider::BoxToSphere(Collider* collider, Collider* other)
{
	//--- ΪΧ»θ
	// SΐW
	Vector3 boxPos = collider->m_trans.lock()->m_pos + collider->m_bound.GetCenter();
	Vector3 spherePos = other->m_trans.lock()->m_pos + other->m_bound.GetCenter();
	// {bNXΜΕεΕ¬
	Vector3 boxMax = boxPos + Vector3(collider->m_bound.GetMax() * collider->m_trans.lock()->m_scale);
	Vector3 boxMin = boxPos + Vector3(collider->m_bound.GetMin() * collider->m_trans.lock()->m_scale);
	// Όa
	float radius = other->m_bound.GetRadius() * other->m_trans.lock()->m_scale->x;

	// »θ
	bool bCol = false;

	// £
	Vector3 maxLen = spherePos - boxMax;
	Vector3	minLen = boxMin - spherePos;

	//--- »θ
	if (maxLen->x <= radius && minLen->x <= radius &&
		maxLen->y <= radius && minLen->y <= radius &&
		maxLen->z <= radius && minLen->z <= radius)
	{
		bCol = true;
	}


	//--- ΅o΅
	// gK[ON
	if (collider->m_bTriggr || other->m_bTriggr) return bCol;

	// e²£
	Vector3 len;
	Vector3 pos;

	// X²
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
	// Y²
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
	// Z²
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

	//--- ΕZ£²πo·
	// X²
	if (len->x <= len->y && len->x <= len->z)
	{
		// ΅o΅
		other->m_trans.lock()->m_pos->x = pos->x;
	}
	// Y²
	else if (len->y <= len->x && len->y <= len->z)
	{
		// ΅o΅
		other->m_trans.lock()->m_pos->y = pos->y;
	}
	// Z²
	else if (len->z <= len->x && len->z <= len->y)
	{
		// ΅o΅
		other->m_trans.lock()->m_pos->z = pos->z;
	}

	//--- ¨
	return bCol;

}

//========================================
//
//	Ζ{bNX
//
//========================================
bool Collider::SphereToBox(Collider* collider, Collider* other)
{
	//--- ΪΧ»θ
	// SΐW
	Vector3 boxPos = other->m_trans.lock()->m_pos + other->m_bound.GetCenter();
	Vector3 spherePos = collider->m_trans.lock()->m_pos + collider->m_bound.GetCenter();
	// {bNXΜΕεΕ¬
	Vector3 boxMax = boxPos + Vector3(other->m_bound.GetMax() * other->m_trans.lock()->m_scale);
	Vector3 boxMin = boxPos + Vector3(other->m_bound.GetMin() * other->m_trans.lock()->m_scale);
	// Όa
	float radius = collider->m_bound.GetRadius() * collider->m_trans.lock()->m_scale->x;

	// »θ
	bool bCol = false;

	// £
	Vector3 maxLen = spherePos - boxMax;
	Vector3	minLen = boxMin - spherePos;

	//--- »θ
	if (maxLen->x <= radius && minLen->x <= radius &&
		maxLen->y <= radius && minLen->y <= radius &&
		maxLen->z <= radius && minLen->z <= radius)
	{
		bCol = true;
	}


	//--- ΅o΅
	// gK[ON
	if (collider->m_bTriggr || other->m_bTriggr) return bCol;

	// e²£
	Vector3 len;
	Vector3 pos;

	// X²
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
	// Y²
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
	// Z²
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

	//--- ΕZ£²πo·
	// X²
	if (len->x <= len->y && len->x <= len->z)
	{
		// ΅o΅
		collider->m_trans.lock()->m_pos->x = pos->x;
	}
	// Y²
	else if (len->y <= len->x && len->y <= len->z)
	{
		// ΅o΅
		collider->m_trans.lock()->m_pos->y = pos->y;
	}
	// Z²
	else if (len->z <= len->x && len->z <= len->y)
	{
		// ΅o΅
		collider->m_trans.lock()->m_pos->z = pos->z;
	}

	//--- ¨
	return bCol;
}

//========================================
//
//	Ζ
//
//========================================
bool Collider::SphereToSphere(Collider* collider, Collider* other)
{
	//--- ΪΧ»θ
	// SΐW
	Vector3 pos1 = collider->m_trans.lock()->m_pos + collider->m_bound.GetCenter();
	Vector3 pos2 = other->m_trans.lock()->m_pos + other->m_bound.GetCenter();
	// Όa
	float radius1 = collider->m_bound.GetRadius() * collider->m_trans.lock()->m_scale->x;
	float radius2 = other->m_bound.GetRadius() * other->m_trans.lock()->m_scale->x;
	// ΥΛtO
	bool bCol = false;

	// ρ_Τ£
	Vector3 distance = pos1 - pos2;

	//--- ΥΛ»θ
	if (distance.magnitudeNoSqrt() < (radius1 + radius2) + (radius1 + radius2))
	{
		bCol = true;
	}

	//---  ΅o΅
	// gK[
	if (collider->m_bTriggr || other->m_bTriggr) return bCol;

	// ρ_ΤΖQΌaΜ·
	float len = (radius1 + radius2) - distance.magnitude();
	// ΅o·ϋό
	Vector3 vec = distance.normalized() * len;
	// ΅o΅
	collider->m_trans.lock()->m_pos += vec;


	//--- ¨
	return true;

}

