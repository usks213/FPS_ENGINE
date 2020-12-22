//==================================================================
//												IComponent.cpp
//	�R���|�[�l���g�x�[�X
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/21	�R���|�[�l���g�x�[�X�N���X�쐬
//
//===================================================================


//===== �C���N���[�h�� =====
#include "IComponent.h"
#include "../Entity/IEntity.h"

using namespace ECS;


//===== �}�N����` =====


//===== �v���g�^�C�v�錾 =====


//===== �O���[�o���ϐ� =====


//===================================
//
//	�R���X�g���N�^
//
//===================================
IComponent::IComponent()
	: m_nTypeID(-1)
{
}


//===================================
//
//	�f�X�g���N�^
//
//===================================
IComponent::~IComponent()
{
	// �e�̃G���e�B�e�B���擾
	const auto& entity = m_Parent.lock();
	if (!entity) return;

	// ����
	auto itr = std::find(entity->m_ComponentPool.begin(), entity->m_ComponentPool.end(), m_self.lock());

	// �Ȃ�������
	if (entity->m_ComponentPool.end() == itr) return;

	// �e�̃R���|�[�l���g���X�g����폜
	entity->m_ComponentPool.erase(itr);

}

//===================================
//
//	���g�̍폜
//
//===================================
void IComponent::Destroy()
{
	// ���g�̔j��
	Object::Destroy();
}



