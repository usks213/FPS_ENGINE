//==================================================================
//												World.h
//	���[���h�̃x�[�X
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/22	���[���h�N���X�쐬
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include <vector>
#include <memory>
#include "../uniqueTypeID.h"
#include "../System/ISystem.h"
#include <string>

//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	// �O��`
	class EntityManager;
	class WorldManager;

	// ���[���h�N���X
	class World
	{
		friend WorldManager;
	private:
		// �G���e�B�e�B�}�l�[�W���[
		std::unique_ptr<EntityManager> m_pEntityManager;
		// �V�X�e�����X�g
		std::vector<std::unique_ptr<ISystem>> m_SystemList;
		// ���[���h�l�[��
		std::string m_name;

	public:
		// �R���X�g���N�^
		World(std::string name);
		// �f�X�g���N�^
		virtual ~World();

		// ���[���h�̏�����
		virtual void Start(){}
		// ���[���h�̏I��
		virtual void End(){}

		// �V�X�e�����X�g�̍X�V
		void Update();
		// �V�X�e�����X�g�̌�X�V
		void LateUpdate();
		// �V�X�e�����X�g�̕`��
		void Draw();

		// �V�X�e���̒ǉ�
		template<class T>
		void AddSystem();
		// �V�X�e���̎擾
		template<class T>
		T* GetSystem();

		// �G���e�B�e�B�}�l�[�W���[���擾
		EntityManager* GetEntityManager() { return m_pEntityManager.get(); }

	private:
		// �V�X�e�����X�g�̃\�[�g
		void sortSystem();
		// OnCreate�R�[���o�b�N�֐�
		static void CallOnCreate(ISystem* pSystem);

		// �I����
		void OnDestroy();
	};


	// �V�X�e���̒ǉ�
	template<class T>
	void World::AddSystem()
	{
		// �V�K����
		T* ptr = new T(this);
		// �V�X�e��ID�̓o�^
		ptr->SetTypeID<T>();
		// ���X�g�ɒǉ�
		m_SystemList.emplace_back(ptr);
		// �����R�[���o�b�N
		CallOnCreate(ptr);
		// �\�[�g
		sortSystem();
	}

	// �V�X�e���̎擾
	template<class T>
	T* World::GetSystem()
	{
		for (const auto& sys : m_SystemList)
		{
			// �R���|�[�l���gID�𔻒�
			if (sys->GetID() == CUniqueTypeID::Get<T>())
			{
				// ����������L���X�g
				T* buffer = static_cast<T*>(sys.get());
				return buffer;
			}
		}
		// �Ȃ�������
		return nullptr;
	}
}
