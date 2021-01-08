//==================================================================
//												ObjectManager.h
//	�I�u�W�F�N�g�̊Ǘ��N���X
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/21	�I�u�W�F�N�g�}�l�[�W���[�N���X�쐬
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include <list>
#include <map>
#include <unordered_map>
#include "IObject.h"


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class ObjectManager final
	{
	private:
		// �C���X�^���X
		static ObjectManager* g_pInstance;

		// �R���X�g���N�^
		ObjectManager();
		// �f�X�g���N�^
		~ObjectManager();

	public:
		// �C���X�^���X�̎擾
		static ObjectManager* GetInstance() { return g_pInstance; }

		// �C���X�^���X����
		static void Create();
		// �C���X�^���X�j��
		static void Destroy();


	private:
		// �I�u�W�F�N�g�v�[��
		using ObjectPool = std::unordered_map<const IObject*,std::shared_ptr<IObject>>;

		// �I�u�W�F�N�g�v�[��
		ObjectPool m_ObjectList;
		// �I�u�W�F�N�g�j�����X�g
		std::unordered_map<const IObject*, ObjectPool::iterator> m_DestroyList;

	public:
		// �I�u�W�F�N�g�̐���
		template<class T> 
		void AddObjectPool(const std::shared_ptr<T>& obj);

		// �I�u�W�F�N�g�̔j��
		void DestroyObject(const std::shared_ptr<IObject>& obj);
		void DestroyObject(IObject* obj);

		// �f�X�g���C���X�g�̃N���A
		void ClearnUpObject();

	};

	// �I�u�W�F�N�g�̐���
	template<class T>
	void ObjectManager::AddObjectPool(const std::shared_ptr<T>& ptr)
	{
		// �i�[
		//m_ObjectList.push_back(ptr);
		m_ObjectList.emplace(ptr.get(), ptr);

		// ���g���i�[
		ptr->m_self = ptr;

		// ������֐�
		ptr->OnCreate();
	}
}
