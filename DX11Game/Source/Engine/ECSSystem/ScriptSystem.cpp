//==================================================================
//												ScriptSystem.h
//	�X�N���v�g�Ǘ��V�X�e��
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/27	�X�N���v�g�V�X�e���쐬
//
//===================================================================


//===== �C���N���[�h�� =====
#include "ScriptSystem.h"
#include "../ECSCompoent/Script.h"
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
ScriptSystem::ScriptSystem(World* pWorld)
	: System<Script>(pWorld)
{
	// �X�V��
	SetUpdateOrder(UpdateOrder::eScript);
}


//===================================
//
//	�f�X�g���N�^
//
//===================================
ScriptSystem::~ScriptSystem()
{
}


//===================================
//
//	�������R�[���o�b�N
//
//===================================
void ScriptSystem::OnCreate()
{
}

//===================================
//
//	�X�V���R�[���o�b�N
//
//===================================
void ScriptSystem::OnUpdate()
{
	std::for_each(m_ComponentList.begin(), m_ComponentList.end(),
		[](Script* script)
		{
			script->Update();
		});
}

//===================================
//
//	��X�V���R�[���o�b�N
//
//===================================
void ScriptSystem::OnLateUpdate()
{
	std::for_each(m_ComponentList.begin(), m_ComponentList.end(),
		[](Script* script)
		{
			script->LateUpdate();
		});
}

//===================================
//
//	�폜���R�[���o�b�N
//
//===================================
void ScriptSystem::OnDestroy()
{
}
