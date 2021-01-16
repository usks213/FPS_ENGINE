//==================================================================
//								BombCrystalScript.cpp
//	�{�������A�C�e�� 
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/10	�{���N���X�^���X�N���v�g�N���X�쐬
//
//===================================================================


//====== �C���N���[�h�� ======

// ���g
#include "BombCrystalScript.h"

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
#include "BombEffectScript.h"
#include "BombCollisionScript.h"


// �l�[���X�y�[�X
using namespace ECS;


//===== �}�N����` =====



//******************** �X�N���v�g���\�b�h ********************


//========================================
//
//	�J�n��
//
//========================================
void BombCrystalScript::Start()
{
	// ���O�E�^�O
	gameObject().lock()->SetName("BombCrystal");
	gameObject().lock()->SetTag("BombCrystal");

	// �傫��
	m_fScale = 200.0f;
	transform().lock()->m_scale = Vector3(m_fScale, m_fScale, m_fScale);
	transform().lock()->m_rot->x = 90;

	//--- �R���|�[�l���̒ǉ�

	// �C���X�^���V���O�����_���[
	const auto& rn = gameObject().lock()->AddComponent<InstancingMeshRenderer>();
	rn->MakeOctahedron("BombCrystal");
	rn->SetDiffuseColor({ 1,0,1,1 });

	// ECS���W�b�h�{�f�B
	const auto& rb = gameObject().lock()->AddComponent<Rigidbody>();
	rb->SetDrag({ 0,0,0 });
	rb->SetGravityForce({ 0,0,0 });
	rb->SetStaticFriction(0);
	rb->SetDynamicFriction(0);
	rb->SetMass(10);
	rb->SetTorqueDrag({ 0,0,0 });

	// ECS�R���C�_�[
	gameObject().lock()->AddComponent<DeltaCollider>()->SetMain(false);


	// ��������
	m_nExitTime = 1800;
}

//========================================
//
//	�X�V��
//
//========================================
void BombCrystalScript::Update()
{
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
}

//========================================
//
//	��X�V��
//
//========================================
void BombCrystalScript::LateUpdate()
{
}

//========================================
//
//	�I����
//
//========================================
void BombCrystalScript::End()
{
}


//******************** �R�[���o�b�N�֐� ********************

//========================================
//
// ����������
//
//========================================
void BombCrystalScript::OnDeltaCollisionEnter(DeltaCollider* collider)
{
	if (collider->gameObject().lock()->tag() == "Player")
	{
		// �{���G�t�F�N�g����
		const int n = 24;
		//const float f = rand() % 314 / 100.0f;
		for (int i = 0; i < n; ++i)
		{
			float phi = 360.0f / n * i * (XM_PI / 180.0f);

			for (int j = 0; j < n; ++j)
			{
				float theta = 360.0f / n * j * (XM_PI / 180.0f);
				// ���W
				Vector3 pos;
				pos->x = cosf(phi) * cosf(theta);
				pos->y = cosf(phi) * sinf(theta);
				pos->z = sinf(phi);
				pos *= 100;

				// �G�t�F�N�g����
				const auto& obj = Instantiate<GameObject>(pos + transform().lock()->m_pos);
				// �R���|�[�l���g�̒ǉ�
				const auto& effect = obj->AddComponent<BombEffectScript>();
				// ���W�b�h�{�f�B
				const auto& rb = obj->GetComponent<Rigidbody>();
				rb->AddForce(pos.normalized() * 30);
			}
		}

		// �����蔻��͈͐���
		const auto& obj = Instantiate<GameObject>(transform().lock()->m_pos);
		// �R���|�[�l���g�̒ǉ�
		const auto& col = obj->AddComponent<BombCollisionScript>();

		// ���g�̔j��
		Destroy(gameObject().lock());

		// �T�E���h
		CSound::PlaySE("Crystal.wav");
	}
}

//========================================
//
// �������Ă����
//
//========================================
void BombCrystalScript::OnDeltaCollisionStay(DeltaCollider* collider)
{
	if (collider->gameObject().lock()->tag() == "Player")
	{
		// �{���G�t�F�N�g����
		const int n = 24;
		//const float f = rand() % 314 / 100.0f;
		for (int i = 0; i < n; ++i)
		{
			float phi = 360.0f / n * i * (XM_PI / 180.0f);

			for (int j = 0; j < n; ++j)
			{
				float theta = 360.0f / n * j * (XM_PI / 180.0f);
				// ���W
				Vector3 pos;
				pos->x = cosf(phi) * cosf(theta);
				pos->y = cosf(phi) * sinf(theta);
				pos->z = sinf(phi);
				pos *= 100;

				// �G�t�F�N�g����
				const auto& obj = Instantiate<GameObject>(pos + transform().lock()->m_pos);
				// �R���|�[�l���g�̒ǉ�
				const auto& effect = obj->AddComponent<BombEffectScript>();
				// ���W�b�h�{�f�B
				const auto& rb = obj->GetComponent<Rigidbody>();
				rb->AddForce(pos.normalized() * 30);
			}
		}

		// �����蔻��͈͐���
		const auto& obj = Instantiate<GameObject>(transform().lock()->m_pos);
		// �R���|�[�l���g�̒ǉ�
		const auto& col = obj->AddComponent<BombCollisionScript>();

		// ���g�̔j��
		Destroy(gameObject().lock());

		// �T�E���h
		CSound::PlaySE("Crystal.wav");
	}
}

//========================================
//
// ���ꂽ��
//
//========================================
void BombCrystalScript::OnDeltaCollisionExit(DeltaCollider* collider)
{
}

