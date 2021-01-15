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
#include "../Renderer/Camera.h"
#include "../System/Sound.h"

// �R���|�[�l���g
#include "../ECSCompoent/Transform.h"
#include "../ECSCompoent/MeshRenderer.h"

// �X�N���v�g
#include "../../Scripts/PlayerScript.h"
#include "../../Scripts/MakeEnemyScript.h"
#include "../../Scripts/StartCrystalScript.h"
#include "../../Scripts/SkyDomeScript.h"
#include "../../Scripts/DeltaUIScript.h"


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
	const auto & playerScript = player->AddComponent<PlayerScript>();

	// �f���^UI
	const auto& deltaUi = GetEntityManager()->CreateEntity<GameObject>();
	deltaUi->AddComponent<DeltaUIScript>()->SetPlayer(playerScript);

	//// �G�l�~�[���C�J�[
	//const auto& enemyMaker = GetEntityManager()->CreateEntity<GameObject>();
	//enemyMaker->AddComponent<MakeEnemyScript>()->SetPlayer(player);

	// �X�^�[�g�N���X�^��
	const auto& crystal = GetEntityManager()->CreateEntity<GameObject>();
	CCamera::GetMainCamera()->Update();
	Vector3 dir = CCamera::GetMainCamera()->GetForward();
	dir *= 1500;
	dir->y = 700;
	crystal->transform().lock()->m_pos = dir;
	crystal->AddComponent<StartCrystalScript>()->SetPlayer(player);

	// �X�J�C�h�[��
	const auto& sky = GetEntityManager()->CreateEntity<GameObject>();
	const auto& skyRn = sky->AddComponent<MeshRenderer>();
	skyRn->MakeSkyDome("Sky", 100);
	skyRn->SetDiffuseTexture("data/texture/sky02.png");
	skyRn->SetLighting(false);
	skyRn->SetLayer(VIEW_FAR_Z);
	skyRn->SetUpdateLayer(false);
	//skyRn->SetRendererBlendState(BS_ALPHABLEND);
	//sky->transform().lock()->m_scale = Vector3{ VIEW_FAR_Z * 1.5f, VIEW_FAR_Z * 1.5f, VIEW_FAR_Z * 1.5f };
	sky->transform().lock()->m_scale = Vector3{ FOG_FAR_Z * 2.5f, FOG_FAR_Z * 2.5f, FOG_FAR_Z * 2.5f };
	sky->AddComponent<SkyDomeScript>()->SetTarget(player);

	// ��
	const auto& plane = GetEntityManager()->CreateEntity<GameObject>();
	const auto& renderer = plane->AddComponent<MeshRenderer>();
	renderer->MakePlane("plane", 1000, 1000, 500, 500, 1, 1);
	//renderer->SetDiffuseTexture("data/texture/grid.png");
	renderer->SetNormalTexture("data/texture/sampledNormals.jpg");
	renderer->SetAmbientTexture("data/texture/sky02.png");
	//renderer->SetTexSize({ 100, 100, 0 });
	//renderer->UpdateTexMatrix();
	plane->transform().lock()->m_pos->y = -1;
	

}

//===================================
//
//	���[���h�̏I��
//
//===================================
void GameWorld::End()
{
	// BGM
	CSound::StopBGM("GameBGM.mp3");

}
