//==================================================================
//								GameOverScript.h
//	�Q�[���I�[�o�[���o
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/15	�Q�[���I�[�o�[�N���X�쐬
//
//===================================================================


//====== �C���N���[�h�� ======

// ���g
#include "GameOverScript.h"

// �V�X�e��
#include "../Engine/System/input.h"
#include "../Engine/System/Sound.h"

// �}�l�[�W���[
#include "../Engine/ECS/Entity/EntityManager.h"

// �G���e�B�e�B
#include "../Engine/ECSEntity/GameObject.h"

// �R���|�[�l���g
#include "../Engine/ECSCompoent/Transform.h"
#include "../Engine/ECSCompoent/MeshRenderer.h"
#include "../Engine/ECSCompoent/BillboardRenderer.h"
#include "../Engine/ECSCompoent/AssimpRenderer.h"
#include "../Engine/ECSCompoent/SpriteRenderer.h"
#include "../Engine/ECSCompoent/InstancingMeshRenderer.h"
#include "../Engine/ECSCompoent/Rigidbody.h"
#include "../Engine/ECSCompoent/BoxCollider.h"
#include "../Engine/ECSCompoent/SphereCollider.h"

// ECS�R���|�[�l���g
#include "../Engine/ECSCompoent/DeltaCollider.h"

// ECS�V�X�e��
#include "../Engine/ECSSystem/DeltaCollisionSystem.h"

// ���[���h
#include "../Engine/ECS/World/WorldManager.h"
#include "../Engine/ECSWorld/TitleWorld.h"


// �l�[���X�y�[�X
using namespace ECS;


//===== �}�N����` =====



//******************** �X�N���v�g���\�b�h ********************


//========================================
//
//	�J�n��
//
//========================================
void GameOverScript::Start()
{
	// �X�P�[��
	transform().lock()->m_scale = 
		Vector3{ SCREEN_WIDTH, SCREEN_HEIGHT, 0 };

	// �X�v���C�g
	const auto& Spr = gameObject().lock()->AddComponent<SpriteRenderer>();
	Spr->SetDiffuseTexture("data/texture/GameOver.png");
	Spr->SetLayer(Spr->eUI3);
	Spr->SetRendererBlendState(BS_ALPHABLEND);
	m_sprite = Spr;

	m_fAlpha = 0.0f;
	m_fAddAlpha = 1.0f / 60.0f;

	// �T�E���h
	CSound::StopBGM("GameBGM.mp3");
	CSound::PlaySE("GameOver.wav", 1.0f);

}

//========================================
//
//	�X�V��
//
//========================================
void GameOverScript::Update()
{
}

//========================================
//
//	��X�V��
//
//========================================
void GameOverScript::LateUpdate()
{
	m_fAlpha += m_fAddAlpha;
	m_sprite.lock()->SetAlpha(m_fAlpha);

	if (m_fAlpha > 2.0f)
	{
		// �^�C�g����
		// �Q�[���I�[�o�[
		WorldManager::GetInstance()->LoadWorld<TitleWorld>("Title");
	}
}

//========================================
//
//	�I����
//
//========================================
void GameOverScript::End()
{
}

