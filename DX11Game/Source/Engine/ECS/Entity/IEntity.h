//==================================================================
//												IEntity.h
//	�G���e�B�e�B�x�[�X
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/21	�x�[�X�̃G���e�B�e�B�N���X�쐬
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include "../Component/IComponent.h"
#include "../Object/ObjectManager.h"
#include <list>


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	// �O��`
	class EntityManager;

	class IEntity : public Object<IEntity>
	{
		friend IComponent;
		using ComponentPool = std::list<std::shared_ptr<IComponent>>;

	private:
		// �R���|�[�l���g�v�[��
		ComponentPool m_ComponentPool;
		// �e�̃G���e�B�e�B�}�l�[�W���[
		EntityManager* m_pEntityManager;

	public:
		// �R���X�g���N�^
		explicit IEntity(EntityManager* entityManager);
		// �f�X�g���N�^
		virtual ~IEntity();
		
		// �R���|�[�l���g�̒ǉ�
		template<class T>
		std::shared_ptr<T> AddComponent();
		// �R���|�[�l���g�̎擾
		template<class T>
		std::shared_ptr<T> GetComponent();
		// �R���|�[�l���g�̔j��
		template<class T>
		bool RemoveComponent();

		// ���g�̔j��
		void Destroy() override;
		// ���b�Z�[�W���M
		void SendComponentMessage(const std::string& message, void* value = nullptr);

		// �e�̃G���e�B�e�B�}�l�[�W���[���擾
		EntityManager* GetEntityManager() { return m_pEntityManager; }
	};

	// �R���|�[�l���g�ǉ�
	template <class T>
	std::shared_ptr<T> IEntity::AddComponent()
	{
		// �R���|�[�l���g�̐V�K�쐬
		std::shared_ptr<T> buffer(new T());
		// �R���|�[�l���gID��o�^
		buffer->SetTypeID<T>();
		// �e�I�u�W�F�N�g��o�^
		buffer->m_Parent = m_self;
		// �e�̃G���e�B�e�B�}�l�[�W���[��o�^
		buffer->m_pEntityManager = m_pEntityManager;
		// ���X�g�Ɋi�[
		m_ComponentPool.push_front(buffer);
		// �I�u�W�F�N�g�v�[���Ɋi�[
		ECS::ObjectManager::GetInstance()->AddObjectPool(buffer);
		// ������
		//buffer->Start();

		return buffer;
	}

	// �R���|�[�l���g���擾
	template <class T>
	std::shared_ptr<T> IEntity::GetComponent()
	{
		for (const auto& com : m_ComponentPool)
		{
			// �R���|�[�l���gID�𔻒�
			if (com->GetID() == CUniqueTypeID::Get<T>())
			{
				// ����������L���X�g
				auto buffer = std::static_pointer_cast<T>(com);

				return buffer;
			}
		}
		// �Ȃ�������
		return nullptr;
	}

	// �R���|�[�l���g������
	template <class T>
	bool IEntity::RemoveComponent()
	{
		for (auto itr = m_ComponentPool.begin(); itr != m_ComponentPool.end(); ++itr)
		{
			// �R���|�[�l���gID�𔻒�
			if ((*itr)->GetID() == CUniqueTypeID::Get<T>())
			{
				// �R���|�[�l���g�̍폜
				(*itr)->Destroy();

				return true;
			}
		}
		return false;
	}
}
