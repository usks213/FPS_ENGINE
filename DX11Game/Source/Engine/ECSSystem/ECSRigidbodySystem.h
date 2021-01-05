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

#pragma once

//====== �C���N���[�h�� ======
#include "../ECS/System/ECSSystem.h"


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	// �O��`
	class RigidbodyData;

	class ECSRigidbodySystem final : public ECSSystem<RigidbodyData>
	{
	public:
		// �R���X�g���N�^
		explicit ECSRigidbodySystem(World* pWorld);
		// �f�X�g���N�^
		virtual ~ECSRigidbodySystem();

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
