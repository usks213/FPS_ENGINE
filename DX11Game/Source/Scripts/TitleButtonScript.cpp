//==================================================================
//								TitleButtonScript.cpp
//	�^�C�g���{�^�����C�A�E�g
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/15	�^�C�g���{�^���N���X�쐬
//
//===================================================================


//====== �C���N���[�h�� ======

// ���g
#include "TitleButtonScript.h"

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
#include "../Engine/ECSWorld/GameWorld.h"

// �l�[���X�y�[�X
using namespace ECS;


//===== �}�N����` =====



//******************** �X�N���v�g���\�b�h ********************


//========================================
//
//	�J�n��
//
//========================================
void TitleButtonScript::Start()
{
	// �T�C�Y
	transform().lock()->m_scale = Vector3{ 1920 / 4 * SCREEN_SCALE_X, 320 / 4 * SCREEN_SCALE_Y, 1 };

	// �X�v���C�g
	const auto& spr = gameObject().lock()->AddComponent<SpriteRenderer>();
	spr->SetRendererBlendState(BS_ALPHABLEND);
	spr->SetDiffuseTexture("data/texture/TitleButton.png");
	spr->SetLayer(spr->eUI1);
	spr->SetAlpha(1);
	
	m_sprite = spr;
}

//========================================
//
//	�X�V��
//
//========================================
void TitleButtonScript::Update()
{
	// �V�[���J��
	if (GetMouseRelease(MOUSEBUTTON_L) || GetMouseRelease(MOUSEBUTTON_R) ||
		GetKeyRelease(VK_RETURN) || GetKeyRelease(VK_SPACE))
	{
		// �Q�[���X�^�[�g
		WorldManager::GetInstance()->LoadWorld<GameWorld>("Game");
	}
}

//========================================
//
//	��X�V��
//
//========================================
void TitleButtonScript::LateUpdate()
{
	// �_�ŏ���
	m_nCount++;

	if (m_nCount % 90 < 60)
	{
		// �\��
		m_sprite.lock()->SetAlpha(1);
	}
	else
	{
		// ��\��
		m_sprite.lock()->SetAlpha(0);
	}
}

//========================================
//
//	�I����
//
//========================================
void TitleButtonScript::End()
{
}

