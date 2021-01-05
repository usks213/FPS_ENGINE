//==================================================================
//												IComponentData.h
//	�f�[�^�w���p�R���|�[�l���g�f�[�^�x�[�X
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/05	�R���|�[�l���g�f�[�^�x�[�X�N���X�쐬
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include "../Object/IObject.h"
#include <string>


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	// �O��`
	class EntityManager;
	class IEntity;

	class IComponentData : public IObject
	{
		friend IEntity;
	public:
		// �e�̃G���e�B�e�B
		std::weak_ptr<IEntity> m_Parent;
		// �e�̃G���e�B�e�B�}�l�[�W���[
		EntityManager* m_pEntityManager;

	public:
		// �R���X�g���N�^
		IComponentData();
		// �f�X�g���N�^
		virtual ~IComponentData();

		// ���g�̔j��
		void Destroy() override;
		// ���b�Z�[�W����M
		virtual void SendComponentMessage(const std::string& message, void* value = nullptr) {};

		// �e�̃G���e�B�e�B�̎擾
		std::weak_ptr<IEntity> GetParent() { return m_Parent; }
		// �e�̃G���e�B�e�B�}�l�[�W���[���擾
		EntityManager* GetEntityManager() { return m_pEntityManager; }
	};

}
