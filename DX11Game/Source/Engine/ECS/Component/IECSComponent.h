//==================================================================
//												IECSComponent.h
//	�f�[�^�w���p�R���|�[�l���g�x�[�X
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/05	�f�[�^�w���ŃR���|�[�l���g�x�[�X�N���X�쐬
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include "IComponent.h"
#include "../Entity/EntityManager.h"

//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	template<class DATA, class SYSTEM>
	class IECSComponent : public IComponent
	{
	public:
		// �I�u�W�F�N�g������
		void OnCreate() override
		{
			// �V�X�e������f�[�^��ǉ��쐬
			m_data = GetEntityManager()->GetWorld()->
				GetSystem<SYSTEM>()->CreateComponentData();

			// �e�I�u�W�F�N�g��o�^
			m_data->m_Parent = m_Parent;
			// �e�̃G���e�B�e�B�}�l�[�W���[��o�^
			m_data->m_pEntityManager = m_pEntityManager;
			// ������֐�
			m_data->OnCreate();
		}

		// �I�u�W�F�N�g�j����
		void OnDestroy() override
		{
			// �V�X�e������f�[�^���폜
			GetEntityManager()->GetWorld()->
				GetSystem<SYSTEM>()->DestroyComponentData(m_data);
		}

		// ���b�Z�[�W��M
		void SendComponentMessage(const std::string& message, void* value) override
		{
			// �f�[�^���Ƀ��b�Z�[�W���M
			m_data->SendComponentMessage(message, value);
		}

		// �f�[�^�擾
		DATA* GetData() { return m_data; }

		// ���Z�q
		DATA* operator -> ()
		{
			return GetData();
		}

	private:
		// �R���|�[�l���g�f�[�^
		DATA* m_data;
	};
}
