//==================================================================
//												RendererSystem.h
//	�����_���[�Ǘ��V�X�e��
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/23	�����_���[�V�X�e���쐬
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
	class Renderer;

	class RendererSystem final : public System<Renderer>
	{
	public:
		// �R���X�g���N�^
		explicit RendererSystem(World* pWorld);
		// �f�X�g���N�^
		virtual ~RendererSystem();

		// ������
		void OnCreate() override;
		// �`��
		void OnDraw() override;
		// �폜��
		void OnDestroy() override;
	private:
		// �\�[�g�p
		static bool swapR(Renderer* lhs, Renderer* rhs);
	};
}
