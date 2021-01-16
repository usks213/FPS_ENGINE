//==================================================================
//								EnemyBaseScript.h
//	�G�l�~�[�x�[�X�X�N���v�g
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/09	�G�l�~�[�x�[�X�N���X�쐬
//
//===================================================================


//====== �C���N���[�h�� ======

// ���g
#include "EnemyBaseScript.h"

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

// �X�N���v�g
#include "DropDeltaScript.h"
#include "EnemyEffectScript.h"


// �l�[���X�y�[�X
using namespace ECS;


//===== �}�N����` =====
#define ADD_SCALE (200.0f / 10.0f)


//******************** �X�N���v�g���\�b�h ********************


//========================================
//
//	�J�n��
//
//========================================
void EnemyBaseScript::Start()
{
	// ���O�E�^�O
	gameObject().lock()->SetName("Enemy");
	gameObject().lock()->SetTag("Enemy");

	// �傫��
	m_fMaxScale = 200;
	m_fScale = 1.0f;
	transform().lock()->m_scale = Vector3(m_fScale, m_fScale, m_fScale);


	//--- �R���|�[�l���̒ǉ�

	// ECS���W�b�h�{�f�B
	const auto& rb = gameObject().lock()->AddComponent<Rigidbody>();
	rb->SetDrag({ 0,0,0 });
	rb->SetGravityForce({ 0,0,0 });
	rb->SetStaticFriction(0);
	rb->SetDynamicFriction(0);
	rb->SetMass(1);
	rb->SetTorqueDrag({ 0,0,0 });
	// ��]
	Vector3 v = { rand() % 100 / 100.0f, rand() % 100 / 100.0f, rand() % 100 / 100.0f };
	v = v.normalized();
	rb->AddTorque(v * 3);
	// ���W�b�h�{�f�B�ۑ�
	m_rb = rb;

	// ECS�R���C�_�[
	gameObject().lock()->AddComponent<DeltaCollider>()->SetMain(false);


	// �ړ����x
	m_speed = 18;
}

//========================================
//
//	�X�V��
//
//========================================
void EnemyBaseScript::Update()
{
	// �傫�������X�ɑ傫������
	if (m_fScale < m_fMaxScale)
	{
		m_fScale += ADD_SCALE;
		if (m_fScale > m_fMaxScale)
		{
			m_fScale = m_fMaxScale;
		}

		// �傫���𔽉f
		transform().lock()->m_scale = Vector3(m_fScale, m_fScale, m_fScale);
	}
}

//========================================
//
//	��X�V��
//
//========================================
void EnemyBaseScript::LateUpdate()
{
}

//========================================
//
//	�I����
//
//========================================
void EnemyBaseScript::End()
{
}


//******************** �R�[���o�b�N�֐� ********************

//========================================
//
// ����������
//
//========================================
void EnemyBaseScript::OnDeltaCollisionEnter(DeltaCollider* collider)
{
	if (collider->gameObject().lock()->tag() == "Player" || 
		collider->gameObject().lock()->tag() == "Bullet")
	{
		// �h���b�v�A�C�e���̐���
		const auto& obj = Instantiate<GameObject>(transform().lock()->m_pos);
		obj->AddComponent<DropDeltaScript>()->SetPlayer(m_player.lock());

		// �G�t�F�N�g�̐���
		const auto& effect = Instantiate<GameObject>(transform().lock()->m_pos);
		effect->AddComponent<EnemyEffectScript>();

		// �폜
		Destroy(gameObject().lock());
	}
}

//========================================
//
// �������Ă����
//
//========================================
void EnemyBaseScript::OnDeltaCollisionStay(DeltaCollider* collider)
{
	if (collider->gameObject().lock()->tag() == "Player" ||
		collider->gameObject().lock()->tag() == "Bullet")
	{
		// �h���b�v�A�C�e���̐���
		const auto& obj = Instantiate<GameObject>(transform().lock()->m_pos);
		obj->AddComponent<DropDeltaScript>()->SetPlayer(m_player.lock());

		// �폜
		Destroy(gameObject().lock());
	}
}

//========================================
//
// ���ꂽ��
//
//========================================
void EnemyBaseScript::OnDeltaCollisionExit(DeltaCollider* collider)
{
}

