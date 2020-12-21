//==================================================================
//												IEntity.cpp
//	�G���e�B�e�B�x�[�X
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/21	�x�[�X�̃G���e�B�e�B�N���X�쐬
//
//===================================================================


//===== �C���N���[�h�� =====
#include "IEntity.h"
#include "EntityManager.h"
#include <algorithm>

using namespace ECS;


//===== �}�N����` =====


//===== �v���g�^�C�v�錾 =====


//===== �O���[�o���ϐ� =====


//===================================
//
//	�R���X�g���N�^
//
//===================================
IEntity::IEntity()
{
}


//===================================
//
//	�f�X�g���N�^
//
//===================================
IEntity::~IEntity()
{
}

//===================================
//
//	���g�̔j��
//
//===================================
void IEntity::Destroy()
{
	//IEntityManager::GetInstance()->DestroyIEntity(this);
	Object::Destroy();
}

//===================================
//
//	�R���|�[�l���g�Ƀ��b�Z�[�W���M
//
//===================================
void IEntity::SendComponentMessage(std::string message)
{
	// ���b�Z�[�W����
	std::for_each(m_ComponentPool.begin(), m_ComponentPool.end(),
		[message](const auto& com)
		{
			com->SendMessage(message);
		});

	//for (const auto& com : m_ComponentPool)
	//{
	//	com->SendMessage(message);
	//}
}
