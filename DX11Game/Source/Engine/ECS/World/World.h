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


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	// �O��`
	class EntityManager;
	class System;

	// ���[���h�N���X
	class World
	{
	private:
		// �G���e�B�e�B�}�l�[�W���[
		std::unique_ptr<EntityManager> m_pEntityManager;
		// �V�X�e�����X�g
		std::vector<std::unique_ptr<System>> m_SystemList;

	public:
		// �R���X�g���N�^
		World();
		// �f�X�g���N�^
		virtual ~World();

		// �V�X�e�����X�g�̍X�V
		void Update();

		// �V�X�e���̒ǉ�
		template<class T>
		void AddSystem()
		{
			// �V�K����
			const auto& ptr = new T(this);
			// ���X�g�ɒǉ�
			m_SystemList.emplace_back(ptr);
			// �����R�[���o�b�N
			CallOnCreate(ptr);
			// �\�[�g
			sortSystem();
		}

		// �G���e�B�e�B�}�l�[�W���[���擾
		EntityManager* GetEntityManager() { return m_pEntityManager.get(); }

	private:
		// �V�X�e�����X�g�̃\�[�g
		void sortSystem();
		// OnCreate�R�[���o�b�N�֐�
		static void CallOnCreate(System* pSystem);
	};

}
