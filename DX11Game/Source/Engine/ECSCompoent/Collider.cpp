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
	m_bCurState(false), m_bOldState(false), m_bTriggr(false)
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
	Vector3 scale1 = collider->m_transform.lock()->m_scale;
	Vector3 scale2 = other->m_transform.lock()->m_scale;
	// �ő�
	Vector3 max1 = pos1 + Vector3(bound1.GetMax() * scale1);
	Vector3 max2 = pos2 + Vector3(bound2.GetMax() * scale2);
	// �ŏ�
	Vector3 min1 = pos1 + Vector3(bound1.GetMin() * scale1);
	Vector3 min2 = pos2 + Vector3(bound2.GetMin() * scale2);

	// �Փ˔���
	//if (min1->x > max2->x) return false;
	//if (max1->x < min2->x) return false;
	//if (min1->y > max2->y) return false;
	//if (max1->y < min2->y) return false;
	//if (min1->z > max2->z) return false;
	//if (max1->z < min2->z) return false;

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
	// �����AABB�Ŏ���Ă���
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

	// ����
	bool bCol = true;

	// ����
	Vector3 maxLen = boxMax2 - boxMin1;
	Vector3	minLen = boxMax1 - boxMin2;


	//--- �����o��
	// �g���K�[ON
	if (collider->m_bTriggr || other->m_bTriggr) return bCol;

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

	//--- �ŒZ���������o��
	// X��
	if (len->x <= len->y && len->x <= len->z)
	{
		// �����o��
		collider->m_transform.lock()->m_pos->x = pos->x;
		// �@��
		normal->y = 0.0f;
		normal->z = 0.0f;
	}
	// Y��
	else if (len->y <= len->x && len->y <= len->z)
	{
		// �����o��
		collider->m_transform.lock()->m_pos->y = pos->y;
		// �@��
		normal->x = 0.0f;
		normal->z = 0.0f;
	}
	// Z��
	else if (len->z <= len->x && len->z <= len->y)
	{
		// �����o��
		collider->m_transform.lock()->m_pos->z = pos->z;
		// �@��
		normal->x = 0.0f;
		normal->y = 0.0f;
	}

	//--- ����

	// ���W�b�h�{�f�B�擾
	const auto& rb1 = collider->rigidbody().lock();
	const auto& rb2 = other->rigidbody().lock();
	if (!rb1 || !rb2) return bCol;
	// ���������邩
	if (!rb1->m_bUsePhysics) return bCol;


	// �x�N�g���̑傫��
	float magnitude = rb1->m_velocity.magnitude();
	// �ǂ���x�N�g��
	Vector3 scratch = Vector3::WallScratchVector(rb1->m_velocity, normal) * magnitude;
	// �����x�N�g��
	Vector3 vertical = Vector3::WallVerticalVector(rb1->m_velocity, normal) * magnitude;
	// ���W12�x�N�g��
	//Vector3 c = rb1->transform().lock()->m_pos - rb2->transform().lock()->m_pos;
	//c = Vector3::WallVerticalVector(c, normal) * c.magnitude();


	//--- ��������(����)
	float m = -rb2->m_fMass / (rb1->m_fMass + rb2->m_fMass);
	float e = (rb1->m_e * rb2->m_e);
	//float d = Vector3::Dot(rb2->m_velocity - vertical, c);
	Vector3 verticalVector = vertical * m * e;


	//--- ��������(���C)
	// ���������̗�
	Vector3 horizontalVector;
	// �����R��
	Vector3 N = vertical;
	// �Î~���C
	float myu_s = rb1->m_fStaticFriction * rb2->m_fStaticFriction;
	// �ő�Î~���C�͂��傫����
	if (scratch.magnitude() > myu_s * N.magnitude())
	{
		// �����C�Ōv�Z
		float myu_d = rb1->m_fDynamicFriction * rb2->m_fDynamicFriction;
		float F_d = myu_d * N.magnitude();
		if (F_d > 1.0f) F_d = 1.0f;
		// ���������̗�
		horizontalVector = scratch - scratch * F_d;
	}


	//--- �x�N�g���̍���
	Vector3 force = horizontalVector + verticalVector;
	// �ŏI�I�ȗ�
	rb1->m_force = force;

	return bCol;
}

//========================================
//
//	�{�b�N�X�Ƌ�
//
//========================================
bool Collider::BoxToSphere(Collider* collider, Collider* other)
{
	// �f�o�b�N
	return BoxToBox(collider, other);

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

	// ����
	bool bCol = false;

	// ����
	Vector3 maxLen = spherePos - boxMax;
	Vector3	minLen = boxMin - spherePos;

	//--- ����
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

	//--- �����o��
	// �g���K�[ON
	if (collider->m_bTriggr || other->m_bTriggr) return bCol;

	// �e������
	Vector3 len;
	Vector3 pos;

	// X��
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
	// Y��
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
	// Z��
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

	//--- �ŒZ���������o��
	// X��
	if (len->x <= len->y && len->x <= len->z)
	{
		// �����o��
		collider->m_transform.lock()->m_pos->x = pos->x;
	}
	// Y��
	else if (len->y <= len->x && len->y <= len->z)
	{
		// �����o��
		collider->m_transform.lock()->m_pos->y = pos->y;
	}
	// Z��
	else if (len->z <= len->x && len->z <= len->y)
	{
		// �����o��
		collider->m_transform.lock()->m_pos->z = pos->z;
	}

	//--- ����
	return bCol;

}

//========================================
//
//	���ƃ{�b�N�X
//
//========================================
bool Collider::SphereToBox(Collider* collider, Collider* other)
{
	// �f�o�b�N
	return BoxToBox(collider, other);

	//--- �ڍה���
	// ���S���W
	Vector3 boxPos = other->m_transform.lock()->m_pos + other->m_bound.GetCenter();
	Vector3 spherePos = collider->m_transform.lock()->m_pos + collider->m_bound.GetCenter();
	// �{�b�N�X�̍ő�ŏ�
	Vector3 boxMax = boxPos + Vector3(other->m_bound.GetMax() * other->m_transform.lock()->m_scale);
	Vector3 boxMin = boxPos + Vector3(other->m_bound.GetMin() * other->m_transform.lock()->m_scale);
	// ���a
	float radius = collider->m_bound.GetRadius() * collider->m_transform.lock()->m_scale->x;

	// ����
	bool bCol = false;

	// ����
	Vector3 maxLen = spherePos - boxMax;
	Vector3	minLen = boxMin - spherePos;

	//--- ����
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

	//--- �����o��
	// �g���K�[ON
	if (collider->m_bTriggr || other->m_bTriggr) return bCol;

	// �e������
	Vector3 len;
	Vector3 pos;

	// X��
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
	// Y��
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
	// Z��
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

	//--- �ŒZ���������o��
	// X��
	if (len->x <= len->y && len->x <= len->z)
	{
		// �����o��
		collider->m_transform.lock()->m_pos->x = pos->x;
	}
	// Y��
	else if (len->y <= len->x && len->y <= len->z)
	{
		// �����o��
		collider->m_transform.lock()->m_pos->y = pos->y;
	}
	// Z��
	else if (len->z <= len->x && len->z <= len->y)
	{
		// �����o��
		collider->m_transform.lock()->m_pos->z = pos->z;
	}

	//--- ����
	return bCol;

	return false;
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
	// �Փ˃t���O
	bool bCol = false;

	// ��_�ԋ���
	Vector3 distance = pos1 - pos2;

	//--- �Փ˔���
	if (distance.magnitudeNoSqrt() < (radius1 + radius2) * (radius1 + radius2))
	{
		bCol = true;
	}
	else
	{
		return false;
	}

	//---  �����o��
	// �g���K�[
	if (collider->m_bTriggr || other->m_bTriggr) return bCol;

	// ��_�ԂƂQ���a�̍�
	float len = (radius1 + radius2) - distance.magnitude();
	// �����o������
	Vector3 vec = distance.normalized() * len;
	// �����o��
	collider->m_transform.lock()->m_pos += vec;


	//--- ����
	return true;

}
