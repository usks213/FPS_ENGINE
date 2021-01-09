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
#include "../Engine/ECSCompoent/ECSRigidbody.h"
#include "../Engine/ECSCompoent/ECSSphereCollider.h"

// ECS�V�X�e��
#include "../Engine/ECSSystem/ECSRigidbodySystem.h"
#include "../Engine/ECSSystem/ECSSphereCollisionSystem.h"


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
	transform().lock()->m_scale = Vector3(100, 100, 100);
	transform().lock()->m_rot->x = 90;

	//--- �R���|�[�l���̒ǉ�

	// �C���X�^���V���O�����_���[
	gameObject().lock()->AddComponent<InstancingMeshRenderer>()->MakeTetraheron("DropDelta");

	// ECS���W�b�h�{�f�B
	const auto& rb = gameObject().lock()->AddComponent<ECSRigidbody>();
	rb->GetData()->SetStaticFriction(0);
	rb->GetData()->SetDynamicFriction(0);
	rb->GetData()->SetMass(1);
	rb->GetData()->SetTorqueDrag({ 0,0,0 });
	// ��]
	rb->GetData()->AddTorqueY(rand() % 3 + 1);


	// ECS�R���C�_�[
	gameObject().lock()->AddComponent<ECSSphereCollider>()->GetData()->SetMain(false);


	// ��������
	m_nExitTime = 300;
}

//========================================
//
//	�X�V��
//
//========================================
void DropDeltaScript::Update()
{
	m_nExitTime--;
	if (m_nExitTime < 0)
	{
		// ���g�̍폜
		GetEntityManager()->DestroyEntity(gameObject().lock());
	}
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
void DropDeltaScript::OnCollisionEnter(Collider* collider)
{

}

//========================================
//
// �������Ă����
//
//========================================
void DropDeltaScript::OnCollisionStay(Collider* collider)
{

}

//========================================
//
// ���ꂽ��
//
//========================================
void DropDeltaScript::OnCollisionExit(Collider* collider)
{

}


//===== ECS =====

//========================================
//
// ����������
//
//========================================
void DropDeltaScript::OnECSCollisionEnter(SphereColliderData* collider)
{
	if (collider->gameObject().lock()->tag() == "Player")
	{
		// ���g�̍폜
		GetEntityManager()->DestroyEntity(gameObject().lock());
	}
}

//========================================
//
// �������Ă����
//
//========================================
void DropDeltaScript::OnECSCollisionStay(SphereColliderData* collider)
{
	if (collider->gameObject().lock()->tag() == "Player")
	{
		// ���g�̍폜
		GetEntityManager()->DestroyEntity(gameObject().lock());
	}
}

//========================================
//
// ���ꂽ��
//
//========================================
void DropDeltaScript::OnECSCollisionExit(SphereColliderData* collider)
{
}

