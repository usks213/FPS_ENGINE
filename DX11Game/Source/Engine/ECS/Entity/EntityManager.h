//==================================================================
//												EntityManager.h
//	�G���e�B�e�B�̊Ǘ��N���X
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
#include "IEntity.h"


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class EntityManager final
	{
	public:
		// �R���X�g���N�^
		explicit EntityManager(World* pWorld);
		// �f�X�g���N�^
		~EntityManager();

	private:
		// �G���e�B�e�B�v�[��
		using EntityPool = std::list<std::weak_ptr<IEntity>>;
		// �G���e�B�e�B�v�[��
		EntityPool m_EntityList;

		// �e�̃��[���h
		World* m_pWorld;

	public:
		// �G���e�B�e�B�̐���
		template<class T> 
		std::shared_ptr<T> CreateEntity();

		// �G���e�B�e�B�̔j��
		void DestroyEntity(const std::shared_ptr<IEntity>& obj);
		void DestroyEntity(IEntity* obj);

		// �G���e�B�e�B�̊i�[
		void AddEntityPool(const std::weak_ptr<IEntity>& entity);
		// �G���e�B�e�B�̏��O
		void RemoveEntityPool(const std::weak_ptr<IEntity>& entity);
		
		// �G���e�B�e�B�v�[���̃N���A
		void ClearEntityPool();

		// �G���e�B�e�B�̐����擾
		int GetEntityCount() { return static_cast<int>(m_EntityList.size()); }
		// ���[���h�̎擾
		World* GetWorld() { return m_pWorld; }
	};

	// �G���e�B�e�B�̐���
	template<class T>
	std::shared_ptr<T> EntityManager::CreateEntity()
	{
		// �G���e�B�e�B�̐���
		const auto& ptr = std::shared_ptr<T>(new T(this));

		// �G���e�B�e�B�v�[���ɓo�^
		AddEntityPool(ptr);

		// �I�u�W�F�N�g�v�[���ɓo�^
		ObjectManager::GetInstance()->AddObjectPool(ptr);

		return ptr;
	}
}
