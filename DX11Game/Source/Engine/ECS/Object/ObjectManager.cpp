//==================================================================
//												ObjectManager.cpp
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


//===== �C���N���[�h�� =====
#include "ObjectManager.h"
#include <algorithm>

using namespace ECS;


//===== �}�N����` =====


//===== �v���g�^�C�v�錾 =====


//===== �O���[�o���ϐ� =====
ObjectManager* ObjectManager::g_pInstance = nullptr;



//===================================
//
//	�R���X�g���N�^
//
//===================================
ObjectManager::ObjectManager()
{
}


//===================================
//
//	�f�X�g���N�^
//
//===================================
ObjectManager::~ObjectManager()
{
}


//===================================
//
//	�C���X�^���X����
//
//===================================
void ObjectManager::Create()
{
	// �m�F
	if (nullptr != g_pInstance) return;

	// ����
	g_pInstance = new ObjectManager();
}


//===================================
//
//	�C���X�^���X�j��
//
//===================================
void ObjectManager::Destroy()
{
	// �m�F
	if (nullptr == g_pInstance) return;

	// �j��
	delete g_pInstance;
	g_pInstance = nullptr;
}


//===================================
//
//	�I�u�W�F�N�g�̔j��
//
//===================================
void ObjectManager::DestroyObject(const std::shared_ptr<IObject>& obj)
{
	// �v�[��������
	auto itr = std::find(m_ObjectList.begin(), m_ObjectList.end(), obj);

	// �v�[���ɂȂ�����
	if (m_ObjectList.end() == itr) return;

	// �f�X�g���C���X�g������
	auto destroyItr = std::find(m_DestroyList.begin(), m_DestroyList.end(), itr);

	// ���Ɋi�[����Ă�����
	if (m_DestroyList.end() != destroyItr) return;

	// �f�X�g���C���X�g�Ɋi�[
	m_DestroyList.push_back(itr);
}

//===================================
//
//	�I�u�W�F�N�g�̔j��
//
//===================================
void ObjectManager::DestroyObject(IObject* obj)
{
	// �v�[��������
	auto itr = std::find_if(m_ObjectList.begin(), m_ObjectList.end(),
		[obj](std::shared_ptr<IObject> obj_s)
		{
			return obj_s.get() == obj;
		});

	// �v�[���ɂȂ�����
	if (m_ObjectList.end() == itr) return;

	// �f�X�g���C���X�g������
	auto destroyItr = std::find(m_DestroyList.begin(), m_DestroyList.end(), itr);

	// ���Ɋi�[����Ă�����
	if (m_DestroyList.end() != destroyItr) return;

	// �f�X�g���C���X�g�Ɋi�[
	m_DestroyList.push_back(itr);
}


//===================================
//
//	�f�X�g���C���X�g�̃N���A(�I�u�W�F�N�g�̔j��)
//
//===================================
void ObjectManager::ClearnUpObject()
{
	// �I�u�W�F�N�g�̔j��
	std::for_each(m_DestroyList.begin(), m_DestroyList.end(),
		[this](const ObjectPool::iterator& itr)
		{
			//�폜�����s�֐�
			(*itr)->OnDestroy();

			// ���S����
			m_ObjectList.erase(itr);
		});

	// ���X�g���N���A
	m_DestroyList.clear();
}