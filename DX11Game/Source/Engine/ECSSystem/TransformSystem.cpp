//==================================================================
//												TransformSystem.cpp
//	�g�����X�t�H�[���Ǘ��V�X�e��
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/23	�g�����X�t�H�[���V�X�e���쐬
//
//===================================================================


//===== �C���N���[�h�� =====
#include "TransformSystem.h"
#include "../ECSCompoent/Transform.h"
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
TransformSystem::TransformSystem(World* pWorld)
	: System<Transform>(pWorld)
{
	// �X�V��
	SetUpdateOrder(UpdateOrder::eTransform);
}


//===================================
//
//	�f�X�g���N�^
//
//===================================
TransformSystem::~TransformSystem()
{
}


//===================================
//
//	�������R�[���o�b�N
//
//===================================
void TransformSystem::OnCreate()
{
}

//===================================
//
//	�X�V���R�[���o�b�N
//
//===================================
void TransformSystem::OnUpdate()
{
	std::for_each(m_ComponentList.begin(), m_ComponentList.end(),
		[](Transform* trans)
		{
			trans->UpdateMatrix();
		});
}

//===================================
//
//	�폜���R�[���o�b�N
//
//===================================
void TransformSystem::OnDestroy()
{
}
