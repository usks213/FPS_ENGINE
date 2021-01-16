//==================================================================
//												TitleWorld.cpp
//	�^�C�g�����[���h
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/15	�^�C�g�����[���h�N���X�쐬
//
//===================================================================


//===== �C���N���[�h�� =====
#include "TitleWorld.h"

// �V�X�e��
#include "../ECS/Entity/EntityManager.h"
#include "../ECSEntity/GameObject.h"
#include "../Renderer/Camera.h"
#include "../System/Sound.h"
#include "../System/Input.h"
#include "../Renderer/PostProcessing.h"

// �R���|�[�l���g
#include "../ECSCompoent/Transform.h"
#include "../ECSCompoent/SpriteRenderer.h"

// �X�N���v�g
#include "../../Scripts/TitleScoreUIScript.h"
#include "../../Scripts/TitleButtonScript.h"


using namespace ECS;


//===== �}�N����` =====


//===== �v���g�^�C�v�錾 =====


//===== �O���[�o���ϐ� =====


//===================================
//
//	�R���X�g���N�^
//
//===================================
TitleWorld::TitleWorld(std::string name)
	:World(name)
{
}

//===================================
//
//	�f�X�g���N�^
//
//===================================
TitleWorld::~TitleWorld()
{
}

//===================================
//
//	���[���h�̏�����
//
//===================================
void TitleWorld::Start()
{
	// �|�X�g�G�t�F�N�g
	PostProcessing::GetMain()->GetColor()->y = 1.0f;
	PostProcessing::GetMain()->GetColor()->z = 1.0f;

	// �^�C�g��
	const auto& title = GetEntityManager()->CreateEntity<GameObject>();
	const auto& titleSpr = title->AddComponent<SpriteRenderer>();
	titleSpr->SetDiffuseTexture("data/texture/FadeDelta.png");
	titleSpr->SetLayer(titleSpr->eBG1);
	title->transform().lock()->m_pos->y = SCREEN_CENTER_Y / 6;
	title->transform().lock()->m_scale = Vector3{ SCREEN_WIDTH, SCREEN_HEIGHT, 1 };

	// �X�R�A
	const auto& score = GetEntityManager()->CreateEntity<GameObject>();
	score->AddComponent<TitleScoreUIScript>();

	// �^�C�g���{�^��
	const auto& button = GetEntityManager()->CreateEntity<GameObject>();
	button->AddComponent<TitleButtonScript>();
	button->transform().lock()->m_pos->y = -SCREEN_CENTER_Y / 3;

	// �J�[�\���̕\��
	SetShowCursor(true);

	// �T�E���h
	CSound::PlayBGM("TitleBGM.mp3", 0.15f);
}

//===================================
//
//	���[���h�̏I��
//
//===================================
void TitleWorld::End()
{
	// �T�E���h
	CSound::StopBGM("TitleBGM.mp3");
}
