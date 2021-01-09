//==================================================================
//								DeltaDeltaCollider.cpp
//	�f���^��p�R���C�_�[
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/09	�f���^��p�R���C�_�[�x�[�X�N���X�쐬
//
//===================================================================


//====== �C���N���[�h�� ======
#include "DeltaCollider.h"
#include "../ECSSystem/DeltaCollisionSystem.h"

#include "../ECS/Entity/EntityManager.h"

#include "../ECSEntity/GameObject.h"
#include "Transform.h"
#include "Rigidbody.h"

#include <math.h>

using namespace ECS;

//===== �}�N����` =====



//===== �v���g�^�C�v�錾 =====


//***************** DeltaCollider ******************


//========================================
//
//	�R���X�g���N�^
//
//========================================
DeltaCollider::DeltaCollider()
	:m_bound(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f)),
	m_bCurState(false), m_bOldState(false), m_bMain(true)
{
}

//========================================
//
//	�f�X�g���N�^
//
//========================================
DeltaCollider::~DeltaCollider()
{
}

//========================================
//
//	������
//
//========================================
void DeltaCollider::OnCreate()
{
	// �V�X�e���Ɋi�[
	DeltaCollisionSystem* sys = GetEntityManager()->GetWorld()->GetSystem<DeltaCollisionSystem>();
	if (sys) sys->AddList(this);

	// �g�����X�t�H�[���擾
	m_transform = m_Parent.lock()->GetComponent<Transform>();
	// �Q�[���I�u�W�F�N�g�擾
	m_gameObject = m_transform.lock()->gameObject();
}

//========================================
//
//	�j����
//
//========================================
void DeltaCollider::OnDestroy()
{
	// �V�X�e�����珜�O
	DeltaCollisionSystem* sys = GetEntityManager()->GetWorld()->GetSystem<DeltaCollisionSystem>();
	if (sys) sys->RemoveList(this);
}

//========================================
//
//	���b�Z�[�W��M
//
//========================================
void DeltaCollider::SendComponentMessage(const std::string& message, void* value)
{
}

//========================================
//
// ���Ƌ��̓����蔻��
//
//========================================
bool DeltaCollider::SpheretoSphere(DeltaCollider* collider, DeltaCollider* other)
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
	return true;
}