//==================================================================
//								DropDeltaScript.cpp
//	�G����h���b�v����A�C�e��
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/09	�h���b�v�f���^�X�N���v�g�N���X�쐬
//
//===================================================================


//====== �C���N���[�h�� ======

// ���g
#include "DropDeltaScript.h"

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



//******************** �X�N���v�g���\�b�h ********************


//========================================
//
//	�J�n��
//
//========================================
void DropDeltaScript::Start()
{
	// ���O�E�^�O
	gameObject().lock()->SetName("DropDelta");
	gameObject().lock()->SetTag("DropDelta");

	// �傫��
	m_fScale = 100.0f;
	transform().lock()->m_scale = Vector3(m_fScale, m_fScale, m_fScale);
	transform().lock()->m_rot->x = 90;

	//--- �R���|�[�l���̒ǉ�

	// �C���X�^���V���O�����_���[
	const auto& render = gameObject().lock()->AddComponent<InstancingMeshRenderer>();
	render->MakeTetraheron("DropDelta");
	render->SetDiffuseColor({ 0,1,0,1 });

	// ECS���W�b�h�{�f�B
	const auto& rb = gameObject().lock()->AddComponent<Rigidbody>();
	rb->SetStaticFriction(0);
	rb->SetDynamicFriction(0);
	rb->SetMass(1);
	rb->SetTorqueDrag({ 0,0,0 });
	// ��]
	rb->AddTorqueY(rand() % 3 + 1);
	m_rb = rb;

	// ECS�R���C�_�[
	gameObject().lock()->AddComponent<DeltaCollider>()->SetMain(false);


	// ��������
	m_nExitTime = 600;
	// �ǔ��I�t
	m_IsTracking = false;
}

//========================================
//
//	�X�V��
//
//========================================
void DropDeltaScript::Update()
{
	// ����
	m_nExitTime--;
	if (m_nExitTime < 0)
	{
		// ���g�̍폜
		GetEntityManager()->DestroyEntity(gameObject().lock());
	}
	else if (m_nExitTime < 120)
	{
		if (m_nExitTime % 16 < 8)
		{
			// ������
			transform().lock()->m_scale = Vector3(0, 0, 0);
		}
		else
		{
			// ������
			transform().lock()->m_scale = Vector3(m_fScale, m_fScale, m_fScale);
		}
	}

	// �n�ʂɒ����Ă���ǔ�
	if (transform().lock()->m_pos->y <= transform().lock()->m_scale->y / 2)
	{
		m_IsTracking = true;
	}
	if (!m_IsTracking) return;

	// �߂��v���C���[�ɒǔ�
	const auto& player = m_player.lock();
	if (!player) return;

	Vector3 vec = player->transform().lock()->m_pos - transform().lock()->m_pos;
	// ��苗���ȉ��Ȃ�
	if (vec.magnitude() > 1000) return;

	// �v���C���[�ɋ߂Â�
	m_rb.lock()->AddForce(vec.normalized() * 3);

}

//========================================
//
//	��X�V��
//
//========================================
void DropDeltaScript::LateUpdate()
{
}

//========================================
//
//	�I����
//
//========================================
void DropDeltaScript::End()
{
}


//******************** �R�[���o�b�N�֐� ********************

//========================================
//
// ����������
//
//========================================
void DropDeltaScript::OnDeltaCollisionEnter(DeltaCollider* collider)
{
	if (collider->gameObject().lock()->tag() == "Player")
	{
		// ���g�̍폜
		GetEntityManager()->DestroyEntity(gameObject().lock());
		// �T�E���h
		CSound::PlaySE("Delta.wav");
	}
}

//========================================
//
// �������Ă����
//
//========================================
void DropDeltaScript::OnDeltaCollisionStay(DeltaCollider* collider)
{
	if (collider->gameObject().lock()->tag() == "Player")
	{
		// ���g�̍폜
		GetEntityManager()->DestroyEntity(gameObject().lock());
		// �T�E���h
		CSound::PlaySE("Delta.wav");
	}
}

//========================================
//
// ���ꂽ��
//
//========================================
void DropDeltaScript::OnDeltaCollisionExit(DeltaCollider* collider)
{
}

