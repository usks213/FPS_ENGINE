//==================================================================
//								SphereCollider.cpp
//	�X�t�B�A�R���C�_�[
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/27	�X�t�B�A�R���C�_�[�N���X�쐬
//
//===================================================================


//====== �C���N���[�h�� ======
#include "SphereCollider.h"
#include "../ECSSystem/CollisionSystem.h"

#include "../ECS/Entity/EntityManager.h"

using namespace ECS;

//===== �}�N����` =====


//========================================
//
//	�R���X�g���N�^
//
//========================================
SphereCollider::SphereCollider()
{
	m_eType = EColliderType::eSphere;
}

//========================================
//
//	�f�X�g���N�^
//
//========================================
SphereCollider::~SphereCollider()
{
}

//========================================
//
//	�Փ˔���
//
//========================================
bool SphereCollider::Judgment(Collider* other)
{
	// ����̃R���C�_�[�ŕ���
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