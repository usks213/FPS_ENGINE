//==================================================================
//												ECSSphereCollisionSystem.h
//	�f�[�^�w���ŋ������蔻��Ǘ��V�X�e��
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/01/06	�f�[�^�w���łō쐬
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
	class SphereColliderData;

	class ECSSphereCollisionSystem final : public ECSSystem<SphereColliderData>
	{
	public:
		// �R���X�g���N�^
		explicit ECSSphereCollisionSystem(World* pWorld);
		// �f�X�g���N�^
		virtual ~ECSSphereCollisionSystem();

		// ������
		void OnCreate() override;
		// �X�V
		void OnUpdate() override;
		// �폜��
		void OnDestroy() override;

	private:
		// �����蔻��
		static void Collision(SphereColliderData* main, const std::list<SphereColliderData*>& m_pSubList);
	};
}
