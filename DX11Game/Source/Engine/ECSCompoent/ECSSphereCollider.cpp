//==================================================================
//								SphereColliderData.cpp
//	�f�[�^�w���ŋ������蔻��R���|�[�l���g
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/06	�f�[�^�w���ō쐬
//
//===================================================================


//====== �C���N���[�h�� ======
#include "ECSSphereCollider.h"
#include "../ECSSystem/ECSSphereCollisionSystem.h"

#include "../ECS/Entity/EntityManager.h"
#include "Transform.h"
#include "Collider.h"


using namespace ECS;

//===== �}�N����` =====


//========================================
//
//	�R���X�g���N�^
//
//========================================
SphereColliderData::SphereColliderData()
	:m_bound(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f)),
	m_bCurState(false), m_bOldState(false),m_bMain(true)
{
	
}

//========================================
//
//	�f�X�g���N�^
//
//========================================
SphereColliderData::~SphereColliderData()
{

}

//========================================
//
//	������
//
//========================================
void SphereColliderData::OnCreate()
{
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
void SphereColliderData::OnDestroy()
{
}

//========================================
//
//	���b�Z�[�W��M
//
//========================================
void SphereColliderData::SendComponentMessage(const std::string& message, void* value)
{
}

//========================================
//
// ���Ƌ��̓����蔻��
//
//========================================
bool SphereColliderData::SpheretoSphere(SphereColliderData* collider, SphereColliderData* other)
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