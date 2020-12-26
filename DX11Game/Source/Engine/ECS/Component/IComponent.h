//==================================================================
//												IComponent.h
//	�R���|�[�l���g�x�[�X
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/21	�R���|�[�l���g�x�[�X�N���X�쐬
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include "../Object/Object.h"
#include "../uniqueTypeID.h"
#include <string>


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	// �O��`
	class EntityManager;
	class IEntity;

	class IComponent : public Object<IComponent>
	{
		friend IEntity;
	protected:
		// �e�̃G���e�B�e�B
		std::weak_ptr<IEntity> m_Parent;
		// �e�̃G���e�B�e�B�}�l�[�W���[
		EntityManager* m_pEntityManager;
	private:
		// �^ID
		int m_nTypeID;

	public:
		// �R���X�g���N�^
		IComponent();
		// �f�X�g���N�^
		virtual ~IComponent();

		// ���g�̔j��
		void Destroy() override;
		// ���b�Z�[�W����M
		virtual void SendComponentMessage(const std::string& message, void* value = nullptr) {};

		// ID�̓o�^
		template <class T>
		void SetTypeID() { m_nTypeID = CUniqueTypeID::Get<T>(); }
		// ID�̎擾
		int GetID() { return m_nTypeID; }

		// �e�̃G���e�B�e�B�̎擾
		std::weak_ptr<IEntity> GetParent() { return m_Parent; }
		// �e�̃G���e�B�e�B�}�l�[�W���[���擾
		EntityManager* GetEntityManager() { return m_pEntityManager; }
	};

}
