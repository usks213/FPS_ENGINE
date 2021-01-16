//==================================================================
//								StartCrystalScript.cpp
//	�Q�[���J�n�N���X�^�� 
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/13	�X�^�[�g�N���X�^���X�N���v�g�N���X�쐬
//
//===================================================================


//====== �C���N���[�h�� ======

// ���g
#include "StartCrystalScript.h"

// ���[���h
#include "../Engine/ECS/World/WorldManager.h"
#include "../Engine/ECSWorld/GameWorld.h"

// �V�X�e��
#include "../Engine/System/input.h"
#include "../Engine/System/Sound.h"
#include "../Engine/System/debugproc.h"

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
#include "MakeEnemyScript.h"

// �l�[���X�y�[�X
using namespace ECS;


//===== �}�N����` =====



//******************** �X�N���v�g���\�b�h ********************


//========================================
//
//	�J�n��
//
//========================================
void StartCrystalScript::Start()
{
	// ���O�E�^�O
	gameObject().lock()->SetName("StartCrystal");
	gameObject().lock()->SetTag("StartCrystal");

	// �X�P�[��
	m_fScale = 300.0f;
	transform().lock()->m_scale = Vector3{ m_fScale,m_fScale,m_fScale };
	transform().lock()->m_rot->x = 90;

	//--- �R���|�[�l���̒ǉ�

	// �C���X�^���V���O�����_���[
	const auto& rn = gameObject().lock()->AddComponent<InstancingMeshRenderer>();
	rn->MakeOctahedron("StartCrystal");
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

	// �p����
	//BombCrystalScript::Start();

	// ��������
	m_nExitTime = 3600;
}

//========================================
//
//	��X�V
//
//========================================
void StartCrystalScript::LateUpdate()
{
	Vector3 pos = transform().lock()->m_pos;
}

//========================================
//
//	�I����
//
//========================================
void StartCrystalScript::End()
{
	// �p����
	BombCrystalScript::End();

	// �Q�[���X�^�[�g
	if (m_bStart)
	{
		const auto& obj = Instantiate<GameObject>();
		const auto& maker = obj->AddComponent<MakeEnemyScript>();
		maker->SetPlayer(m_player.lock());
	}
	// ���[�h������
	else
	{
		WorldManager::GetInstance()->LoadWorld<GameWorld>("Game");
	}
}


//******************** �R�[���o�b�N�֐� ********************

//========================================
//
// ����������
//
//========================================
void StartCrystalScript::OnDeltaCollisionEnter(DeltaCollider* collider)
{
	// �p����
	BombCrystalScript::OnDeltaCollisionEnter(collider);

	if (collider->gameObject().lock()->tag() == "Player")
	{
		m_bStart = true;
	}
}

//========================================
//
// �������Ă����
//
//========================================
void StartCrystalScript::OnDeltaCollisionStay(DeltaCollider* collider)
{
	// �p����
	BombCrystalScript::OnDeltaCollisionStay(collider);

	if (collider->gameObject().lock()->tag() == "Player")
	{
		m_bStart = true;
	}
}
