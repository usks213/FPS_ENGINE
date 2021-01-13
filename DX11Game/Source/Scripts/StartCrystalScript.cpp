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
	// �p����
	BombCrystalScript::Start();

	// ���O�E�^�O
	gameObject().lock()->SetName("StartCrystal");
	gameObject().lock()->SetTag("StartCrystal");

	// �X�P�[��
	transform().lock()->m_scale = Vector3{ 300,300,300 };

	// ��������
	m_nExitTime = 3600;
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