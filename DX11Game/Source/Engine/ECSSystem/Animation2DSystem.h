//==================================================================
//												Animation2DSystem.h
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

#pragma once

//====== �C���N���[�h�� ======
#include "../ECS/System/System.h"


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	// �O��`
	class Animator2D;

	class Animation2DSystem final : public System<Animator2D>
	{
	public:
		// �R���X�g���N�^
		explicit Animation2DSystem(World* pWorld);
		// �f�X�g���N�^
		virtual ~Animation2DSystem();

		// ������
		void OnCreate() override;
		// �X�V
		void OnUpdate() override;
		// �폜��
		void OnDestroy() override;
	};
}
