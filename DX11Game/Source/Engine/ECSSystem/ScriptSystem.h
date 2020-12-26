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

#pragma once

//====== �C���N���[�h�� ======
#include "../ECS/System/System.h"


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	// �O��`
	class Script;

	class ScriptSystem final : public System<Script>
	{
	public:
		// �R���X�g���N�^
		explicit ScriptSystem(World* pWorld);
		// �f�X�g���N�^
		virtual ~ScriptSystem();

		// ������
		void OnCreate() override;
		// �X�V
		void OnUpdate() override;
		// ��X�V
		void OnLateUpdate() override;
		// �폜��
		void OnDestroy() override;
	};
}
