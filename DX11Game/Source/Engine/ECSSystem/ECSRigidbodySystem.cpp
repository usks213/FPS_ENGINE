//==================================================================
//												ECSRigidbodySystem.h
//	�f�[�^�w���Ń��W�b�h�{�f�B�Ǘ��V�X�e��
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/01/05	�f�[�^�w���łō쐬
//
//===================================================================


//===== �C���N���[�h�� =====
#include "ECSRigidbodySystem.h"
#include "../ECSCompoent/ECSRigidbody.h"
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
ECSRigidbodySystem::ECSRigidbodySystem(World* pWorld)
	: ECSSystem<RigidbodyData>(pWorld)
{
	// �X�V��
	SetUpdateOrder(UpdateOrder::eRigidbody);
}


//===================================
//
//	�f�X�g���N�^
//
//===================================
ECSRigidbodySystem::~ECSRigidbodySystem()
{
}


//===================================
//
//	�������R�[���o�b�N
//
//===================================
void ECSRigidbodySystem::OnCreate()
{
}

//===================================
//
//	�X�V���R�[���o�b�N
//
//===================================
void ECSRigidbodySystem::OnUpdate()
{
	std::for_each(m_ComponentList.begin(), m_ComponentList.end(),
		[](RigidbodyData& rb)
		{
			rb.Update();
		});
}

//===================================
//
//	��X�V���R�[���o�b�N
//
//===================================
void ECSRigidbodySystem::OnLateUpdate()
{
	std::for_each(m_ComponentList.begin(), m_ComponentList.end(),
		[](RigidbodyData& rb)
		{
			rb.LateUpdate();
		});
}

//===================================
//
//	�폜���R�[���o�b�N
//
//===================================
void ECSRigidbodySystem::OnDestroy()
{
}
