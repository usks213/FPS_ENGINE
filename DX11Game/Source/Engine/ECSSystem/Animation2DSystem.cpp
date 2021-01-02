//==================================================================
//												Animation2DSystem.cpp
//	2D�A�j���[�V�����Ǘ��V�X�e��
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/23	�A�j���[�V����2D�V�X�e���쐬
//
//===================================================================


//===== �C���N���[�h�� =====
#include "Animation2DSystem.h"
#include "../ECSCompoent/Animator2D.h"
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
Animation2DSystem::Animation2DSystem(World* pWorld)
	: System<Animator2D>(pWorld)
{
	// �X�V��
	SetUpdateOrder(UpdateOrder::eAnimation2D);
}


//===================================
//
//	�f�X�g���N�^
//
//===================================
Animation2DSystem::~Animation2DSystem()
{
}


//===================================
//
//	�������R�[���o�b�N
//
//===================================
void Animation2DSystem::OnCreate()
{
}

//===================================
//
//	�X�V���R�[���o�b�N
//
//===================================
void Animation2DSystem::OnUpdate()
{
	std::for_each(m_ComponentList.begin(), m_ComponentList.end(),
		[](Animator2D* anim)
		{
			anim->AnimationUpdate();
		});
}

//===================================
//
//	�폜���R�[���o�b�N
//
//===================================
void Animation2DSystem::OnDestroy()
{
}
