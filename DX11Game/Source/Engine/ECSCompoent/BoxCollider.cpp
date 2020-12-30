//==================================================================
//								BoxCollider.cpp
//	�{�b�N�X�R���C�_�[
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/27	�{�b�N�X�R���C�_�[�N���X�쐬
//
//===================================================================


//====== �C���N���[�h�� ======
#include "BoxCollider.h"
#include "../ECSSystem/CollisionSystem.h"

#include "../ECS/Entity/EntityManager.h"

using namespace ECS;

//===== �}�N����` =====


//========================================
//
//	�R���X�g���N�^
//
//========================================
BoxCollider::BoxCollider()
{
	m_eType = EColliderType::eBox;
}

//========================================
//
//	�f�X�g���N�^
//
//========================================
BoxCollider::~BoxCollider()
{
}

//========================================
//
//	�Փ˔���
//
//========================================
bool BoxCollider::Judgment(Collider* other)
{
	// ����̃R���C�_�[�ŕ���
	switch (other->GetColliderType())
	{
	case EColliderType::eBox:
		return BoxToBox(this, other);
		break;
	case EColliderType::eSphere:
		return BoxToSphere(this, other);
		break;
	default:
		break;
	}

	return false;
}