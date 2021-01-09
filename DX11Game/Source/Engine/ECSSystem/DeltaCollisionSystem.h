//==================================================================
//												DeltaCollisionSystem.h
//	�f���^��p�����蔻��Ǘ��V�X�e��
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/09	�f���^�R���W�����V�X�e���쐬
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
	class DeltaCollider;

	class DeltaCollisionSystem final : public System<DeltaCollider>
	{
	public:
		// �R���X�g���N�^
		explicit DeltaCollisionSystem(World* pWorld);
		// �f�X�g���N�^
		virtual ~DeltaCollisionSystem();

		// ������
		void OnCreate() override;
		// �X�V
		void OnUpdate() override;
		// �폜��
		void OnDestroy() override;

	private:
		// �m�[�h�Ƌ�Ԃ̓����蔻��
		static void Collision(DeltaCollider* main, const std::list<DeltaCollider*>& m_pSubList);
	};
}
