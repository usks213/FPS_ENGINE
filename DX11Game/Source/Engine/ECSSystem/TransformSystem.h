//==================================================================
//												TransformSystem.h
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

#pragma once

//====== �C���N���[�h�� ======
#include "../ECS/System/System.h"


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	// �O��`
	class Transform;

	class TransformSystem final : public System<Transform>
	{
	public:
		// �R���X�g���N�^
		explicit TransformSystem(World* pWorld);
		// �f�X�g���N�^
		virtual ~TransformSystem();

		// ������
		void OnCreate() override;
		// �X�V
		void OnUpdate() override;
		// �폜��
		void OnDestroy() override;
	};
}
