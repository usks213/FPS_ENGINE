//==================================================================
//												RigidbodySystem.h
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

#pragma once

//====== �C���N���[�h�� ======
#include "../ECS/System/System.h"


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	// �O��`
	class Rigidbody;

	class RigidbodySystem final : public System<Rigidbody>
	{
	public:
		// �R���X�g���N�^
		explicit RigidbodySystem(World* pWorld);
		// �f�X�g���N�^
		virtual ~RigidbodySystem();

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
