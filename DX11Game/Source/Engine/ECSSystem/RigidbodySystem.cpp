//==================================================================
//												RigidbodySystem.cpp
//	���W�b�h�{�f�B�Ǘ��V�X�e��
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/27	���W�b�h�{�f�B�V�X�e���쐬
//
//===================================================================


//===== �C���N���[�h�� =====
#include "RigidbodySystem.h"
#include "../ECSCompoent/Rigidbody.h"
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
RigidbodySystem::RigidbodySystem(World* pWorld)
	: System<Rigidbody>(pWorld)
{
	// �X�V��
	SetUpdateOrder(UpdateOrder::eRigidbody);
}


//===================================
//
//	�f�X�g���N�^
//
//===================================
RigidbodySystem::~RigidbodySystem()
{
}


//===================================
//
//	�������R�[���o�b�N
//
//===================================
void RigidbodySystem::OnCreate()
{
}

//===================================
//
//	�X�V���R�[���o�b�N
//
//===================================
void RigidbodySystem::OnUpdate()
{
	std::for_each(m_ComponentList.begin(), m_ComponentList.end(),
		[](Rigidbody* rb)
		{
			rb->Update();
		});
}

//===================================
//
//	��X�V���R�[���o�b�N
//
//===================================
void RigidbodySystem::OnLateUpdate()
{
	std::for_each(m_ComponentList.begin(), m_ComponentList.end(),
		[](Rigidbody* rb)
		{
			rb->LateUpdate();
		});
}

//===================================
//
//	�폜���R�[���o�b�N
//
//===================================
void RigidbodySystem::OnDestroy()
{
}
