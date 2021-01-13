//==================================================================
//												WorldManager.h
//	���[���h�̊Ǘ��N���X
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/11	���[���h�}�l�[�W���[�N���X�쐬
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include <unordered_map>
#include <map>
#include <string>
#include "World.h"
#include "../../../Scripts/FadeScript.h"
#include "../../ECSEntity/GameObject.h"


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class WorldManager final
	{
	private:
		// �C���X�^���X
		static WorldManager* g_pInstance;

		// �R���X�g���N�^
		WorldManager();
		// �f�X�g���N�^
		~WorldManager();

	public:
		// �C���X�^���X�̎擾
		static WorldManager* GetInstance() { return g_pInstance; }

		// �C���X�^���X����
		static void Create();
		// �C���X�^���X�j��
		static void Destroy();

	private:
		// ���[���h�v�[��
		using WorldPool = std::map<const std::string,std::shared_ptr<World>>;

		// ���[���h�v�[��
		WorldPool m_WorldList;
		// ���[���h�j�����X�g
		std::unordered_map<const World*, WorldPool::iterator> m_DestroyList;

		// �}�l�[�W���[�������Ă���B��̃��[���h
		std::unique_ptr<World> m_masterWorld;
		// ���݂̃V�[��
		std::weak_ptr<World> m_currentWorld;
		// ���̃��[���h
		std::weak_ptr<World> m_nextWorld;

		// ���[���h�ύX�t���O
		bool m_bChange;

	public:
		// ������
		void Start();
		// �I��
		void End();
		// �X�V
		void Update();
		// �`��
		void Draw();

		// ���[���h�̐���
		template<class T> 
		std::shared_ptr<T> CreateWorld(const std::string name);
		// ���[���h�̒ǉ�
		void AddWorld(const std::shared_ptr<World>& world ) {
			m_WorldList.emplace(world->m_name, world);}

		// ���[���h�̔j��
		void DestroyWorld(const std::string name);
		void DestroyWorld(const std::shared_ptr<World>& obj);
		void DestroyWorld(World* obj);

		// �f�X�g���C���X�g�̃N���A
		void ClearnUpWorld();

		// �}�X�^�[���[���h�擾
		World* GetMasterWorld() { return m_masterWorld.get(); }

		// ���[���h�ؑ�
		template <class T>
		void LoadWorld(const std::string name);
		// ���̃��[���h�ɕύX
		void SetNextWorld();

	};

	// ���[���h�̐���
	template<class T>
	std::shared_ptr<T> WorldManager::CreateWorld(const std::string name)
	{
		// ����
		auto itr = m_WorldList.find(name);
		// �v�[���ɂ�����
		if (m_WorldList.end() != itr) 
			return std::static_pointer_cast<T>(itr->second);

		// ����
		std::shared_ptr<T> ptr(new T(name));

		// �i�[
		m_WorldList.emplace(name, ptr);

		return ptr;
	}

	template <class T>
	void WorldManager::LoadWorld(const std::string name)
	{
		// ���[���h�ύX��
		if (m_bChange) return;

		// ���̃��[���h�̐���
		m_nextWorld = CreateWorld<T>(name);

		// �������[���h����������
		if (m_currentWorld.lock().get() == m_nextWorld.lock().get())
		{
			// ���[���h�������[�h
			std::shared_ptr<T> ptr(new T(name));
			m_nextWorld = ptr;

			// ���[���h�ύX�t���O�I��
			m_bChange = true;

			// �t�F�[�h�̐���
			const auto& obj = m_masterWorld->GetEntityManager()->CreateEntity<GameObject>();
			const auto& fade = obj->AddComponent<FadeScript>();
			fade->SetFadeEndFunc([this, ptr]() 
				{
				this->SetNextWorld(); 
				this->AddWorld(ptr);
				});
			// �t�F�[�h�̎��s
			fade->StartFadeOut();

			return;
		}

		// ���[���h�ύX�t���O�I��
		m_bChange = true;

		// �t�F�[�h�̐���
		const auto& obj = m_masterWorld->GetEntityManager()->CreateEntity<GameObject>();
		const auto& fade = obj->AddComponent<FadeScript>();
		fade->SetFadeEndFunc([this]() {this->SetNextWorld(); });
		// �t�F�[�h�̎��s
		fade->StartFadeOut();

		// Test 
		//SetNextWorld();
	}
}
