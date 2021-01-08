//==================================================================
//								BulletScript.cpp
//	�o���b�g�X�N���v�g
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/07	�o���b�g�X�N���v�g�N���X�쐬
//
//===================================================================


//====== �C���N���[�h�� ======

// ���g
#include "BulletScript.h"

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
void BulletScript::Start()
{
	// ���O�E�^�O
	gameObject().lock()->SetName("Bullet");
	gameObject().lock()->SetTag("Bullet");

	// �傫��
	transform().lock()->m_scale = Vector3(1000, 1000, 1000);

	//--- �R���|�[�l���̒ǉ�

	// �C���X�^���V���O�����_���[
	gameObject().lock()->AddComponent<InstancingMeshRenderer>()->MakeDodecahedron("Bullet");

	// ECS���W�b�h�{�f�B
	const auto& rb = gameObject().lock()->AddComponent<ECSRigidbody>();
	rb->GetData()->SetDrag({ 0,0,0 });
	rb->GetData()->SetGravityForce({ 0,0,0 });
	rb->GetData()->SetStaticFriction(0);
	rb->GetData()->SetDynamicFriction(0);
	rb->GetData()->SetMass(10);
	rb->GetData()->SetTorqueDrag({ 0,0,0 });

	// ECS�R���C�_�[
	gameObject().lock()->AddComponent<ECSSphereCollider>();


	// ��������
	m_nExitTime = 300;
}

//========================================
//
//	�X�V��
//
//========================================
void BulletScript::Update()
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
void BulletScript::LateUpdate()
{
}

//========================================
//
//	�I����
//
//========================================
void BulletScript::End()
{
}


//******************** �R�[���o�b�N�֐� ********************


//========================================
//
// ����������
//
//========================================
void BulletScript::OnCollisionEnter(Collider* collider)
{

}

//========================================
//
// �������Ă����
//
//========================================
void BulletScript::OnCollisionStay(Collider* collider)
{

}

//========================================
//
// ���ꂽ��
//
//========================================
void BulletScript::OnCollisionExit(Collider* collider)
{

}


//===== ECS =====

//========================================
//
// ����������
//
//========================================
void BulletScript::OnECSCollisionEnter(SphereColliderData* collider)
{
	if (collider->gameObject().lock()->tag() != "Player")
	{
		// �폜
		GetEntityManager()->DestroyEntity(collider->gameObject().lock());
		// ���g�̍폜
		//GetEntityManager()->DestroyEntity(gameObject().lock());
	}
}

//========================================
//
// �������Ă����
//
//========================================
void BulletScript::OnECSCollisionStay(SphereColliderData* collider)
{
	if (collider->gameObject().lock()->tag() != "Player")
	{
		// �폜
		GetEntityManager()->DestroyEntity(collider->gameObject().lock());
		// ���g�̍폜
		//GetEntityManager()->DestroyEntity(gameObject().lock());
	}
}

//========================================
//
// ���ꂽ��
//
//========================================
void BulletScript::OnECSCollisionExit(SphereColliderData* collider)
{
}

