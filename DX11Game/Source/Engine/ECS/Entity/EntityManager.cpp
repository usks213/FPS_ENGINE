//==================================================================
//												EntityManager.cpp
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


//===== �C���N���[�h�� =====
#include "EntityManager.h"
#include <algorithm>

using namespace ECS;


//===== �}�N����` =====


//===== �v���g�^�C�v�錾 =====


//===== �O���[�o���ϐ� =====



//===================================
//
//	�R���X�g���N�^
//
//===================================
EntityManager::EntityManager(World* pWorld)
	: m_pWorld(pWorld)
{
}


//===================================
//
//	�f�X�g���N�^
//
//===================================
EntityManager::~EntityManager()
{
}


//===================================
//
//	�G���e�B�e�B�̔j��
//
//===================================
void EntityManager::DestroyEntity(const std::shared_ptr<IEntity>& entity)
{
	entity->Destroy();
}

//===================================
//
//	�G���e�B�e�B�̔j��
//
//===================================
void EntityManager::DestroyEntity(IEntity* entity)
{
	entity->Destroy();
}


//===================================
//
//	�G���e�B�e�B�̒ǉ�
//
//===================================
void EntityManager::AddEntityPool(const std::weak_ptr<IEntity>& entity)
{
	m_EntityList.push_back(entity);
}


//===================================
//
//	�G���e�B�e�B�̏��O
//
//===================================
void EntityManager::RemoveEntityPool(const std::weak_ptr<IEntity>& entity)
{
	// ��v����G���e�B�e�B�̌���
	auto itr = std::find_if(m_EntityList.begin(), m_EntityList.end(),
		[entity](const std::weak_ptr<IEntity>& entity2)
		{
			return entity.lock().get() == entity2.lock().get();
		});

	// ������Ȃ�����
	if (m_EntityList.end() == itr) return;

	// �v�[������폜
	m_EntityList.erase(itr);
}


//===================================
//
//	�G���e�B�e�B�v�[���̃N���A
//
//===================================
void EntityManager::ClearEntityPool()
{
	// �S�G���e�B�e�B�̔j��
	for (const auto& entity : m_EntityList)
	{
		DestroyEntity(entity.lock());
	}
}