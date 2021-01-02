//==================================================================
//								Collider.h
//	�R���C�_�[�x�[�X
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/24	�R���C�_�[�x�[�X�N���X�쐬
//
//===================================================================


//====== �C���N���[�h�� ======
#include "Collider.h"
#include "../ECSSystem/CollisionSystem.h"

#include "../ECS/Entity/EntityManager.h"

#include "../ECSEntity/GameObject.h"
#include "Transform.h"
#include "Rigidbody.h"

#include <math.h>

using namespace ECS;

//===== �}�N����` =====



//===== �v���g�^�C�v�錾 =====
// AABB�Ɠ_�̍ŒZ����
float LenAABBToPoint(Bounds& box, Vector3& p);
Vector3 LenAABBToPoint(Vector3& min, Vector3& max, Vector3& p);



//***************** Bounds ******************

//========================================
//
//	�R���X�g���N�^
//
//========================================
Bounds::Bounds(Vector3 center, Vector3 size)
	:m_center(center), m_size(size) 
{
}


//***************** Collider ******************


//========================================
//
//	�R���X�g���N�^
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
//	�f�X�g���N�^
//
//========================================
Collider::~Collider()
{

}

//========================================
//
//	������
//
//========================================
void Collider::OnCreate()
{
	// �V�X�e���Ɋi�[
	CollisionSystem* sys = GetEntityManager()->GetWorld()->GetSystem<CollisionSystem>();
	if (sys) sys->AddList(this);

	// �g�����X�t�H�[���擾
	m_transform = m_Parent.lock()->GetComponent<Transform>();
	// �Q�[���I�u�W�F�N�g�擾
	m_gameObject = m_transform.lock()->gameObject();
	// ���W�b�h�{�f�B�擾
	m_rigidbody = m_gameObject.lock()->GetComponent<Rigidbody>();
}

//========================================
//
//	�j����
//
//========================================
void Collider::OnDestroy()
{
	// �V�X�e�����珜�O
	CollisionSystem* sys = GetEntityManager()->GetWorld()->GetSystem<CollisionSystem>();
	if (sys) sys->RemoveList(this);
}

//========================================
//
//	���b�Z�[�W��M
//
//========================================
void Collider::SendComponentMessage(const std::string& message, void* value)
{
	// Update���b�Z�[�W�Ń}�g���b�N�X���X�V
	if ("Update" == message)
	{
	}
}

//========================================
//
//	AABBtoAABB ������p
//
//========================================
bool Collider::AABBtoAABB(Collider* collider, Collider* other)
{
	// �o�E���h
	Bounds& bound1 = collider->m_bound;
	Bounds& bound2 = other->m_bound;

	// ���W
	Vector3 pos1 = collider->m_transform.lock()->m_pos + bound1.GetCenter();
	Vector3 pos2 = other->m_transform.lock()->m_pos + bound2.GetCenter();
	// �X�P�[��
	Vector3& scale1 = collider->m_transform.lock()->m_scale;
	Vector3& scale2 = other->m_transform.lock()->m_scale;
	// �ő�
	Vector3 max1 = pos1 + bound1.GetMax() * scale1;
	Vector3 max2 = pos2 + bound2.GetMax() * scale2;
	// �ŏ�
	Vector3 min1 = pos1 + bound1.GetMin() * scale1;
	Vector3 min2 = pos2 + bound2.GetMin() * scale2;

	// �Փ˔���
	//if (min1->x > max2->x) return false;
	//if (max1->x < min2->x) return false;
	//if (min1->y > max2->y) return false;
	//if (max1->y < min2->y) return false;
	//if (min1->z > max2->z) return false;
	//if (max1->z < min2->z) return false;
	// return true;

	// �Փ˔���
	if (min1->x < max2->x && min2->x < max1->x &&
		min1->y < max2->y && min2->y < max1->y &&
		min1->z < max2->z && min2->z < max1->z)
	{
		return true;
	}

	// ��Փ�
	return false;
}

//========================================
//
//	�{�b�N�X�ƃ{�b�N�X
//
//========================================
bool Collider::BoxToBox(Collider* collider, Collider* other)
{
	//--- �����AABB�Ŏ���Ă���
	// �g���K�[�Ȃ�
	if (collider->m_bTriggr || other->m_bTriggr) return true;
	// ���W�b�h�{�f�B�擾
	const auto& rb1 = collider->rigidbody().lock();
	const auto& rb2 = other->rigidbody().lock();
	if (!rb1 || !rb2) return true;


	//--- �ڍה���
	// ���S���W
	Vector3 boxPos1 = collider->m_transform.lock()->m_pos + collider->m_bound.GetCenter();
	Vector3 boxPos2 = other->m_transform.lock()->m_pos + other->m_bound.GetCenter();
	// �{�b�N�X�̍ő�ŏ�
	Vector3 boxMax1 = boxPos1 + Vector3(collider->m_bound.GetMax() * collider->m_transform.lock()->m_scale);
	Vector3 boxMin1 = boxPos1 + Vector3(collider->m_bound.GetMin() * collider->m_transform.lock()->m_scale);
	Vector3 boxMax2 = boxPos2 + Vector3(other->m_bound.GetMax() * other->m_transform.lock()->m_scale);
	Vector3 boxMin2 = boxPos2 + Vector3(other->m_bound.GetMin() * other->m_transform.lock()->m_scale);

	// �n�[�t�T�C�Y
	Vector3 boxSize1 = collider->m_bound.GetHalfSize() * collider->m_transform.lock()->m_scale;
	Vector3 boxSize2 = other->m_bound.GetHalfSize() * other->m_transform.lock()->m_scale;


	//====================================================================
	// ����
	Vector3 maxLen = boxMax2 - boxMin1;
	Vector3	minLen = boxMax1 - boxMin2;

	// �e������
	Vector3 len;
	Vector3 pos;
	Vector3 normal;

	// X��
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
	// Y��
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
	// Z��
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

	// ����
	len->x = fabsf(len->x);
	len->y = fabsf(len->y);
	len->z = fabsf(len->z);

	//--- �����o��
	//--- �ŒZ���������o��
	// X��
	if (len->x <= len->y && len->x <= len->z)
	{
		if (rb1->m_bUsePhysics)
		{
			// �����o��
			collider->m_transform.lock()->m_pos->x = pos->x;
		}
		else
		{
			if (rb2->m_bUsePhysics)
			// �����
			other->m_transform.lock()->m_pos->x = boxPos1->x - (boxSize1->x + boxSize2->x) * normal->x;
		}
		// �@��
		normal->y = 0.0f;
		normal->z = 0.0f;
	}
	// Y��
	else if (len->y <= len->x && len->y <= len->z)
	{
		if (rb1->m_bUsePhysics)
		{
			// �����o��
			collider->m_transform.lock()->m_pos->y = pos->y; 
		}
		else
		if (rb2->m_bUsePhysics)
		{
			// �����
			other->m_transform.lock()->m_pos->y = boxPos1->y - (boxSize1->y + boxSize2->y) * normal->y;
		}
		// �@��
		normal->x = 0.0f;
		normal->z = 0.0f;
	}
	// Z��
	else if (len->z <= len->x && len->z <= len->y)
	{
		if (rb1->m_bUsePhysics)
		{
			// �����o��
			collider->m_transform.lock()->m_pos->z = pos->z;
		}
		else
		{
			if (rb2->m_bUsePhysics)
			// �����
			other->m_transform.lock()->m_pos->z = boxPos1->z - (boxSize1->z + boxSize2->z) * normal->z;
		}
		// �@��
		normal->x = 0.0f;
		normal->y = 0.0f;
	}

	////--- ����
	//--- ����
	// ���������Ȃ�
	if (rb1->GetUsePhysics())
	{
		// �����v�Z
		rb1->CollisionPhysics(rb2, normal);
	}
	else
	{
		// �����v�Z
		rb2->CollisionPhysics(rb1, normal);
	}

	return true;
}

//========================================
//
//	�{�b�N�X�Ƌ�
//
//========================================
bool Collider::BoxToSphere(Collider* collider, Collider* other)
{
	//--- �ڍה���
		// ���S���W
	Vector3 boxPos = collider->m_transform.lock()->m_pos + collider->m_bound.GetCenter();
	Vector3 spherePos = other->m_transform.lock()->m_pos + other->m_bound.GetCenter();
	// �{�b�N�X�̍ő�ŏ�
	Vector3 boxMax = boxPos + Vector3(collider->m_bound.GetMax() * collider->m_transform.lock()->m_scale);
	Vector3 boxMin = boxPos + Vector3(collider->m_bound.GetMin() * collider->m_transform.lock()->m_scale);
	// ���a
	float radius = other->m_bound.GetRadius() * other->m_transform.lock()->m_scale->x;
	// �n�[�t�T�C�Y
	Vector3 boxSize = collider->m_bound.GetHalfSize() * collider->m_transform.lock()->m_scale;
	// �ŒZ�������v�Z
	Vector3 vlen = LenAABBToPoint(boxMin, boxMax, spherePos);
	float len = vlen.magnitude();

	//--- ����
	// �������ĂȂ�
	if (len >= radius) return false;


	//--- ����
	// �g���K�[�Ȃ�
	if (collider->m_bTriggr || other->m_bTriggr) return true;
	// ���W�b�h�{�f�B�擾
	const auto& rb1 = collider->rigidbody().lock();
	const auto& rb2 = other->rigidbody().lock();
	if (!rb1 || !rb2) return true;

	//// ���������Ȃ�
	//if (!rb1->GetUsePhysics()) return true;
	//// �����v�Z
	//rb1->CollisionPhysics(rb2, vlen.normalized());

	////--- �����o��
	//// ��_�ԂƂQ���a�̍�
	//len = radius - len;
	//// �����o������
	//Vector3 vec = vlen.normalized() * len;
	//// �����o��
	//collider->m_transform.lock()->m_pos -= vec;


	// ���������Ȃ�
	if (rb1->GetUsePhysics())
	{
		// �����v�Z
		rb1->CollisionPhysics(rb2, vlen.normalized());

		//--- �����o��
		// ��_�ԂƂQ���a�̍�
		len = radius - len;
		// �����o������
		Vector3 vec = vlen.normalized() * len;
		// �����o��
		collider->m_transform.lock()->m_pos -= vec;
	}
	else
	{
		if (rb2->GetUsePhysics())
		{
			// �����v�Z
			rb2->CollisionPhysics(rb1, vlen.normalized());

			//--- �����o��
			// ��_�ԂƂQ���a�̍�
			len = radius - len;
			// �����o������
			Vector3 vec = vlen.normalized() * len;
			// �����o��
			other->m_transform.lock()->m_pos += vec;
		}
	}

	return true;
}

//========================================
//
//	���ƃ{�b�N�X
//
//========================================
bool Collider::SphereToBox(Collider* collider, Collider* other)
{
	//--- �ڍה���
	// ���S���W
	Vector3 boxPos = other->m_transform.lock()->m_pos + other->m_bound.GetCenter();
	Vector3 spherePos = collider->m_transform.lock()->m_pos + collider->m_bound.GetCenter();
	// �{�b�N�X�̍ő�ŏ�
	Vector3 boxMax = boxPos + Vector3(other->m_bound.GetMax() * other->m_transform.lock()->m_scale);
	Vector3 boxMin = boxPos + Vector3(other->m_bound.GetMin() * other->m_transform.lock()->m_scale);
	// ���a
	float radius = collider->m_bound.GetRadius() * collider->m_transform.lock()->m_scale->x;
	// �n�[�t�T�C�Y
	Vector3 boxSize = collider->m_bound.GetHalfSize() * collider->m_transform.lock()->m_scale;
	// �ŒZ�������v�Z
	Vector3 vlen = LenAABBToPoint(boxMin, boxMax, spherePos);
	float len = vlen.magnitude();

	//--- ����
	// �������ĂȂ�
	if (len >= radius) return false;

	
	//--- ����
	// �g���K�[�Ȃ�
	if (collider->m_bTriggr || other->m_bTriggr) return true;
	// ���W�b�h�{�f�B�擾
	const auto& rb1 = collider->rigidbody().lock();
	const auto& rb2 = other->rigidbody().lock();
	if (!rb1 || !rb2) return true;

	//// ���������Ȃ�
	//if (!rb1->GetUsePhysics()) return true;
	//// �����v�Z
	//rb1->CollisionPhysics(rb2, vlen.normalized());


	////--- �����o��
	//// ��_�ԂƂQ���a�̍�
	//len = radius - len;
	//// �����o������
	//Vector3 vec = vlen.normalized() * len;
	//// �����o��
	//collider->m_transform.lock()->m_pos += vec;


	// ���������Ȃ�
	if (rb1->GetUsePhysics())
	{
		// �����v�Z
		rb1->CollisionPhysics(rb2, vlen.normalized());

		//--- �����o��
		// ��_�ԂƂQ���a�̍�
		len = radius - len;
		// �����o������
		Vector3 vec = vlen.normalized() * len;
		// �����o��
		collider->m_transform.lock()->m_pos += vec;
	}
	else
	{
		if (rb2->GetUsePhysics())
		{
			// �����v�Z
			rb2->CollisionPhysics(rb1, vlen.normalized());

			//--- �����o��
			// ��_�ԂƂQ���a�̍�
			len = radius - len;
			// �����o������
			Vector3 vec = vlen.normalized() * len;
			// �����o��
			other->m_transform.lock()->m_pos -= vec;
		}
	}

	return true;
}

//========================================
//
//	���Ƌ�
//
//========================================
bool Collider::SphereToSphere(Collider* collider, Collider* other)
{
	//--- �ڍה���
	// ���S���W
	Vector3 pos1 = collider->m_transform.lock()->m_pos + collider->m_bound.GetCenter();
	Vector3 pos2 = other->m_transform.lock()->m_pos + other->m_bound.GetCenter();
	// ���a
	float radius1 = collider->m_bound.GetRadius() * collider->m_transform.lock()->m_scale->x;
	float radius2 = other->m_bound.GetRadius() * other->m_transform.lock()->m_scale->x;

	// ��_�ԋ���
	Vector3 distance = pos1 - pos2;

	//--- �Փ˔���
	if (distance.magnitudeNoSqrt() >= (radius1 + radius2) * (radius1 + radius2)) return false;

	
	//--- ����
	// �g���K�[
	if (collider->m_bTriggr || other->m_bTriggr) return true;
	// ���W�b�h�{�f�B�擾
	const auto& rb1 = collider->rigidbody().lock();
	const auto& rb2 = other->rigidbody().lock();
	if (!rb1 || !rb2) return true;

	// ���������Ȃ�
	if (!rb1->GetUsePhysics()) return true;
	// �����v�Z
	rb1->CollisionPhysics(rb2, distance.normalized());


	//---  �����o��
	// ��_�ԂƂQ���a�̍�
	float len = (radius1 + radius2) - distance.magnitude();
	// �����o������
	Vector3 vec = distance.normalized() * len;
	// �����o��
	collider->m_transform.lock()->m_pos += vec;


	return true;

}

// AABB�Ɠ_�̍ŒZ����
float LenAABBToPoint(Bounds& box, Vector3& p)
{
	// �����ׂ̂���̒l���i�[
	float SqLen = 0;   
	
	// �e���œ_���ŏ��l�ȉ��������͍ő�l�ȏ�Ȃ�΁A�����l��

	// X��
	if (p->x < box.GetMin()->x)  
		SqLen += (p->x - box.GetMin()->x) * (p->x - box.GetMin()->x);
	if (p->x > box.GetMax()->x)
		SqLen += (p->x - box.GetMax()->x) * (p->x - box.GetMax()->x);
	
	// Y��
	if (p->y < box.GetMin()->y)
		SqLen += (p->y - box.GetMin()->y) * (p->y - box.GetMin()->y);
	if (p->y > box.GetMax()->y)
		SqLen += (p->y - box.GetMax()->y) * (p->y - box.GetMax()->y);

	// Z��
	if (p->z < box.GetMin()->z)
		SqLen += (p->z - box.GetMin()->z) * (p->z - box.GetMin()->z);
	if (p->z > box.GetMax()->z)
		SqLen += (p->z - box.GetMax()->z) * (p->z - box.GetMax()->z);

	// ������
	return sqrt(SqLen);
}

// AABB�Ɠ_�̍ŒZ����
Vector3 LenAABBToPoint(Vector3& min, Vector3 &max, Vector3& p)
{
	// �����ׂ̂���̒l���i�[
	Vector3 SqLen;

	// �e���œ_���ŏ��l�ȉ��������͍ő�l�ȏ�Ȃ�΁A�����l��

	// X��
	if (p->x < min->x)
		SqLen->x += (p->x - min->x);
	if (p->x > max->x)
		SqLen->x += (p->x - max->x);

	// Y��
	if (p->y < min->y)
		SqLen->y += (p->y - min->y);
	if (p->y > max->y)
		SqLen->y += (p->y - max->y);

	// Z��
	if (p->z < min->z)
		SqLen->z += (p->z - min->z);
	if (p->z > max->z)
		SqLen->z += (p->z - max->z);

	return SqLen;
}