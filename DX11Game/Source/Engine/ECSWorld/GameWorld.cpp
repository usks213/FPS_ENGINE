//==================================================================
//												GameWorld.cpp
//	�Q�[�����[���h
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/11	�Q�[�����[���h�N���X�쐬
//				�Q�[���V�[�����ڐA
//
//===================================================================


//===== �C���N���[�h�� =====
#include "GameWorld.h"

// �V�X�e��
#include "../ECS/Entity/EntityManager.h"
#include "../ECSEntity/GameObject.h"

// �R���|�[�l���g
#include "../ECSCompoent/Transform.h"
#include "../ECSCompoent/MeshRenderer.h"

// �X�N���v�g
#include "../../Scripts/PlayerScript.h"
#include "../../Scripts/MakeEnemyScript.h"

using namespace ECS;


//===== �}�N����` =====


//===== �v���g�^�C�v�錾 =====


//===== �O���[�o���ϐ� =====


//===================================
//
//	�R���X�g���N�^
//
//===================================
GameWorld::GameWorld(std::string name)
	:World(name)
{
}

//===================================
//
//	�f�X�g���N�^
//
//===================================
GameWorld::~GameWorld()
{
}

//===================================
//
//	���[���h�̏�����
//
//===================================
void GameWorld::Start()
{
	//--- �Q�[���I�u�W�F�N�g��ǉ�

	// �v���C���[
	const auto& player = GetEntityManager()->CreateEntity<GameObject>();
	player->AddComponent<PlayerScript>();

	// �G�l�~�[���C�J�[
	const auto& enemyMaker = GetEntityManager()->CreateEntity<GameObject>();
	enemyMaker->AddComponent<MakeEnemyScript>()->SetPlayer(player);

	// ��
	const auto& test = GetEntityManager()->CreateEntity<GameObject>();
	const auto renderer = test->AddComponent<MeshRenderer>();
	renderer->MakeCube("plane");
	renderer->SetDiffuseTexture("data/texture/grid.png");
	renderer->SetTexSize({ 100, 100, 0 });
	renderer->UpdateTexMatrix();
	test->transform().lock()->m_scale = Vector3{ 100000, 1, 100000 };

}

//===================================
//
//	���[���h�̏I��
//
//===================================
void GameWorld::End()
{
}
