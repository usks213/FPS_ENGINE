//==================================================================
//								FadeScript.cpp
//	�t�F�[�h�X�N���v�g
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/13	�t�F�[�h�X�N���v�g�N���X�쐬
//
//===================================================================


//====== �C���N���[�h�� ======

// ���g
#include "FadeScript.h"

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


// �l�[���X�y�[�X
using namespace ECS;

//===== �}�N����` =====


//===== �� =====
namespace
{
	enum E_STATE_FADE	// �t�F�[�h�̏�Ԃ̗�
	{
		E_STATE_FADE_NODE = 0,
		E_STATE_FADE_OUT,
		E_STATE_FADE_IN,

		E_STATE_FADE_MAX,
	};
}



//******************** �X�N���v�g���\�b�h ********************


//========================================
//
//	�J�n��
//
//========================================
void FadeScript::Start()
{
	// �X�P�[��
	m_nAlpha = 0;
	// �X�e�[�g
	m_nState = E_STATE_FADE_NODE;
	// �t���[��
	m_nFrame = 60;
	// �֐��I�u�W�F�N�g
	m_fadeEndFunc = nullptr;

	// �Q�[���I�u�W�F�N�g
	gameObject().lock()->SetName("Fade");
	gameObject().lock()->SetTag("Fade");

	// �����_���[�擾
	const auto& render = gameObject().lock()->AddComponent<SpriteRenderer>();
	render->SetDiffuseColor({1,1,1});
	render->SetAlpha(m_nAlpha);
	render->SetLayer(render->eFade1);
	render->SetRendererBlendState(EBlendState::BS_ALPHABLEND);
	// �e�N�X�`��
	//render->SetDiffuseTexture("data/texture/FadeDelta.png");
	m_render = render;

	// �T�C�Y
	transform().lock()->m_scale = Vector3{ SCREEN_WIDTH, SCREEN_HEIGHT, 1 };

}

//========================================
//
//	�X�V��
//
//========================================
void FadeScript::Update()
{
}

//========================================
//
//	��X�V��
//
//========================================
void FadeScript::LateUpdate()
{
	// ��Ԃɂ�蕪��
	switch (m_nState)
	{
	case(E_STATE_FADE_IN):		// �t�F�[�h�C��
		// ���l�̌���
		m_nAlpha -= (int)(255.0f / m_nFrame);

		// ���l��0��菬�����Ȃ�����I��
		if (m_nAlpha < 0)
		{
			m_nAlpha = 0;
			m_nState = E_STATE_FADE_NODE;
			m_bNowFade = false;

			// �t�F�[�h�̏���
			Destroy(gameObject().lock());
		}

		// ���l�̍X�V
		m_render.lock()->SetAlpha(m_nAlpha / 255.0f);
		break;

	case(E_STATE_FADE_OUT):		// �t�F�[�h�A�E�g
		// ���l�̑���
		m_nAlpha += (int)(255.0f / m_nFrame);

		// ���l��255�𒴂�����t�F�[�h�C����
		if (m_nAlpha > 255)
		{
			m_nAlpha = 255;
			m_nState = E_STATE_FADE_IN;

			// �t�F�[�h�A�E�g���I������Ƃ��Ɏ������鏈��
			// �V�[���̈ړ�
			if (m_fadeEndFunc) m_fadeEndFunc();

			// �T�E���h

		}

		// ���l�̍X�V
		m_render.lock()->SetAlpha(m_nAlpha / 255.0f);
		break;

	default:
		break;
	}

	// ���l�̍X�V
	m_render.lock()->SetAlpha(m_nAlpha / 255.0f);
}

//========================================
//
//	�I����
//
//========================================
void FadeScript::End()
{
}


//========================================
//
// �t�F�[�h�A�E�g�̊J�n
//
//========================================
void FadeScript::StartFadeOut(int nFrame)
{
	// ���Ƀt�F�[�h���̎��͏������Ȃ�
	if (E_STATE_FADE_NODE != m_nState)
	{
		return;
	}

	m_nAlpha = 0;
	m_nState = E_STATE_FADE_OUT;
	m_bNowFade = true;
}