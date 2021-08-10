//==================================================================
//												World.cpp
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


//===== �C���N���[�h�� =====
#include "World.h"
#include "../Entity/EntityManager.h"
#include "../System/ISystem.h"
#include <algorithm>

// �V�X�e��
#include "../../ECSSystem/TransformSystem.h"
#include "../../ECSSystem/RendererSystem.h"
#include "../../ECSSystem/Animation2DSystem.h"
#include "../../ECSSystem/CollisionSystem.h"
#include "../../ECSSystem/DeltaCollisionSystem.h"
#include "../../ECSSystem/RigidbodySystem.h"
#include "../../ECSSystem/ScriptSystem.h"

using namespace ECS;


//===== �}�N����` =====


//===== �v���g�^�C�v�錾 =====


//===== �O���[�o���ϐ� =====


//===================================
//
//	�R���X�g���N�^
//
//===================================
World::World(std::string name)
	:m_name(name)
{
	// �G���e�B�e�B�}�l�[�W���[�̐���
	m_pEntityManager.reset(new EntityManager(this));

	// �V�X�e���̒ǉ�
	AddSystem<TransformSystem>();
	AddSystem<RendererSystem>();
	AddSystem<Animation2DSystem>();
	AddSystem<CollisionSystem>();
	AddSystem<DeltaCollisionSystem>();
	AddSystem<RigidbodySystem>();
	AddSystem<ScriptSystem>();
}


//===================================
//
//	�f�X�g���N�^
//
//===================================
World::~World()
{
	// �G���e�B�e�B�̔j��
	GetEntityManager()->ClearEntityPool();

	// �I�u�W�F�N�g�̃v�[���̔j��
	ObjectManager::GetInstance()->ClearnUpObject();

	// �V�X�e�����X�g�̏I������
	for (auto&& system : m_SystemList)
	{
		// OnDestroy���Ăяo��
		system->OnDestroy();
	}
}


//===================================
//
//	�V�X�e�����X�g�̍X�V
//
//===================================
void World::Update()
{
	// �V�X�e�����X�g�̍X�V����
	for (auto&& system : m_SystemList)
	{
		// �R���|�[�l���g�̃\�[�g
		//system->SortComponentList();
		// OnUpdate���Ăяo��
		system->OnUpdate();
	}
}

//===================================
//
//	�V�X�e�����X�g�̌�X�V
//
//===================================
void World::LateUpdate()
{
	// �V�X�e�����X�g�̍X�V����
	for (auto&& system : m_SystemList)
	{
		// OnUpdate���Ăяo��
		system->OnLateUpdate();
	}
}

//===================================
//
//	�V�X�e�����X�g�̕`��
//
//===================================
void World::Draw()
{
	// �V�X�e�����X�g�̍X�V����
	for (auto&& system : m_SystemList)
	{
		// OnUpdate���Ăяo��
		system->OnDraw();
	}
}


//===================================
//
//	�V�X�e�����X�g�̃\�[�g
//
//===================================
void World::sortSystem()
{
	std::sort(m_SystemList.begin(), m_SystemList.end(),
		[](const std::unique_ptr<ISystem>& lhs, const std::unique_ptr<ISystem>& rhs)
		{
			// ����
			return lhs->GetUpdateOrder() < rhs->GetUpdateOrder();
		});
}

//= ==================================
//
//	�V�X�e��OnCreate�̃R�[���o�b�N
//
//===================================
void World::CallOnCreate(ISystem* pSystem)
{
	// OnCreate���Ăяo��
	pSystem->OnCreate();
}

//= ==================================
//
//	�I����
//
//===================================
void World::OnDestroy()
{
	// �G���e�B�e�B�̔j��
	GetEntityManager()->ClearEntityPool();

	// �I�u�W�F�N�g�̃v�[���̔j��
	ObjectManager::GetInstance()->ClearnUpObject();

	// �V�X�e�����X�g�̏I������
	for (auto&& system : m_SystemList)
	{
		// OnDestroy���Ăяo��
		system->OnDestroy();
	}
}