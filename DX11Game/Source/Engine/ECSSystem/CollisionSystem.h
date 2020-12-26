//==================================================================
//												CollisionSystem.h
//	�����蔻��Ǘ��V�X�e��
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/27	�R���W�����V�X�e���쐬
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
	class Collider;

	class CollisionSystem final : public System<Collider>
	{
	public:
		// �R���X�g���N�^
		explicit CollisionSystem(World* pWorld);
		// �f�X�g���N�^
		virtual ~CollisionSystem();

		// ������
		void OnCreate() override;
		// �X�V
		void OnUpdate() override;
		// �폜��
		void OnDestroy() override;

	private:
		// �m�[�h�Ƌ�Ԃ̓����蔻��
		static void Collision(Collider* main, const std::list<Collider*>& m_pSubList);
	};
}
