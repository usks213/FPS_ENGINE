//==================================================================
//												System.h
//	�V�X�e���x�[�X
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/22	�G���e�B�e�B�}�l�[�W���[�N���X�쐬
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include <list>
#include "../World/World.h"


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class System final
	{
	public:
		// �R���X�g���N�^
		explicit System(World* pWorld);
		// �f�X�g���N�^
		virtual ~System();

		// ������
		virtual void OnCreate();
		// �X�V
		virtual void OnUpdate() {};
		// �폜��
		virtual void OnDestroy();

		// �X�V�����̎��s�����擾
		int GetUpdateOrder() { return m_nUpdateOrder; }

	protected:
		// �G���e�B�e�B�}�l�[�W���[�̎擾
		EntityManager* GetEntityManager() { return m_pWorld->GetEntityManager(); }

		// �X�V�����̎��s����ݒ�
		void SetUpdateOrder(int nOrder) { m_nUpdateOrder = nOrder; }

	private:
		// �e�̃��[���h
		World* m_pWorld;
		// �X�V�����̎��s��
		int m_nUpdateOrder;
	};
}
