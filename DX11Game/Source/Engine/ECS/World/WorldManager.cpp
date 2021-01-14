//==================================================================
//												WorldManager.cpp
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


//===== �C���N���[�h�� =====
#include "WorldManager.h"
#include <algorithm>
#include "../Object/ObjectManager.h"
#include "../../ECSWorld/MasterWorld.h"


using namespace ECS;


//===== �}�N����` =====


//===== �v���g�^�C�v�錾 =====


//===== �O���[�o���ϐ� =====
WorldManager* WorldManager::g_pInstance = nullptr;



//*************************** �V���O���g�� ***************************

//===================================
//
//	�R���X�g���N�^
//
//===================================
WorldManager::WorldManager()
	:m_masterWorld(new MasterWorld("MasterWorld")),
	m_bChange(false)
{
}


//===================================
//
//	�f�X�g���N�^
//
//===================================
WorldManager::~WorldManager()
{
}


//===================================
//
//	�C���X�^���X����
//
//===================================
void WorldManager::Create()
{
	// �m�F
	if (nullptr != g_pInstance) return;

	// ����
	g_pInstance = new WorldManager();
	g_pInstance->Start();
}


//===================================
//
//	�C���X�^���X�j��
//
//===================================
void WorldManager::Destroy()
{
	// �m�F
	if (nullptr == g_pInstance) return;

	// �j��
	g_pInstance->End();
	delete g_pInstance;
	g_pInstance = nullptr;
}


//*************************** �C���X�^���X ***************************

//===================================
//
//	���[���h�}�l�[�W���[�̏�����
//
//===================================
void WorldManager::Start()
{
	// �}�X�^�[���[���h�̏�����
	if(m_masterWorld)
		m_masterWorld->Start();

	// ���݂̃��[���h�̏�����
	const auto& world = m_currentWorld.lock();
	if (world) world->Start();
}

//===================================
//
//	���[���h�}�l�[�W���[�̏I��
//
//===================================
void WorldManager::End()
{
	// ���݂̃��[���h�̏I��
	const auto& world = m_currentWorld.lock();
	if (world) world->End();

	// �}�X�^�[���[���h�̏I��
	if (m_masterWorld)
		m_masterWorld->End();
}

//===================================
//
//	���[���h�}�l�[�W���[�̍X�V
//
//===================================
void WorldManager::Update()
{
	// ���݂̃��[���h�̍X�V
	const auto& world = m_currentWorld.lock();
	if (world) world->Update();

	// �}�X�^�[���[���h�̍X�V
	if (m_masterWorld)
		m_masterWorld->Update();

	// ���݂̃��[���h�̌�X�V
	if (world) world->LateUpdate();

	// �}�X�^�[���[���h�̌�X�V
	if (m_masterWorld)
		m_masterWorld->LateUpdate();

	// ���[���h�̍폜�X�V
	ClearnUpWorld();
}

//===================================
//
//	���[���h�}�l�[�W���[�̕`��
//
//===================================
void WorldManager::Draw()
{
	// ���݂̃��[���h�̕`��
	const auto& world = m_currentWorld.lock();
	if (world) world->Draw();

	// �}�X�^�[���[���h�̕`��
	if (m_masterWorld)
		m_masterWorld->Draw();
}


//===================================
//
//	���[���h�̔j��
//
//===================================
void WorldManager::DestroyWorld(const std::string name)
{
	// �v�[��������
	auto itr = m_WorldList.find(name);

	// �v�[���ɂȂ�����
	if (m_WorldList.end() == itr) return;

	// �f�X�g���C���X�g������
	auto destroyItr = m_DestroyList.find(itr->second.get());

	// ���Ɋi�[����Ă�����
	if (m_DestroyList.end() != destroyItr) return;

	// �f�X�g���C���X�g�Ɋi�[
	m_DestroyList.emplace(itr->second.get(), itr);
}

//===================================
//
//	���[���h�̔j��
//
//===================================
void WorldManager::DestroyWorld(const std::shared_ptr<World>& obj)
{
	// ���O�Ō���
	DestroyWorld(obj.get());
}

//===================================
//
//	���[���h�̔j��
//
//===================================
void WorldManager::DestroyWorld(World* obj)
{
	// �v�[��������
	auto itr = std::find_if(m_WorldList.begin(), m_WorldList.end(),
		[&obj](const auto& map)
		{
			return map.second.get() == obj;
		});

	// �v�[���ɂȂ�����
	if (m_WorldList.end() == itr) return;

	// �f�X�g���C���X�g������
	auto destroyItr = m_DestroyList.find(itr->second.get());

	// ���Ɋi�[����Ă�����
	if (m_DestroyList.end() != destroyItr) return;

	// �f�X�g���C���X�g�Ɋi�[
	m_DestroyList.emplace(itr->second.get(), itr);
}

//===================================
//
//	�f�X�g���C���X�g�̃N���A(���[���h�̔j��)
//
//===================================
void WorldManager::ClearnUpWorld()
{
	// ���[���h�̔j��
	for (auto obj : m_DestroyList)
	{
		//�폜�����s�֐�
		obj.second->second->OnDestroy();

		// ���S����
		m_WorldList.erase(obj.second);
	}

	// ���X�g���N���A
	m_DestroyList.clear();

	// �I�u�W�F�N�g�̍폜�X�V
	ObjectManager::GetInstance()->ClearnUpObject();
}

//===================================
//
//	���̃��[���h�ɕύX
//
//===================================
void WorldManager::SetNextWorld()
{
	if (!m_currentWorld.expired())
	{
		// ���݂̃V�[���̏I���������Ă�
		m_currentWorld.lock()->End();
		// ���݂̃V�[���̏���
		DestroyWorld(m_currentWorld.lock());
	}

	// ���[���h�̃N���[���A�b�v
	ClearnUpWorld();

	if (!m_nextWorld.expired())
	{
		// ���݂̃V�[���̕ύX
		m_currentWorld = m_nextWorld;
		// ���̃V�[���������
		m_nextWorld.reset();
		// ���̃V�[���̏�����
		m_currentWorld.lock()->Start();
	}

	// �V�[���ύX�t���O�I�t
	m_bChange = false;
}