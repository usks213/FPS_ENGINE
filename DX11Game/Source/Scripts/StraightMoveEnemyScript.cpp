//==================================================================
//								StraightMoveEnemyScript.cpp
//	�X�g���[�g���[�u�G�l�~�[
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/08	�X�g���[�g���[�u�G�l�~�[�N���X�쐬
//
//===================================================================


//====== �C���N���[�h�� ======

// ���g
#include "StraightMoveEnemyScript.h"

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
void StraightMoveEnemyScript::Start()
{
	// ���O�E�^�O
	gameObject().lock()->SetName("StraightMoveEnemy");
	gameObject().lock()->SetTag("Enemy");

	// �傫��
	transform().lock()->m_scale = Vector3(200, 200, 200);

	//--- �R���|�[�l���̒ǉ�

	// �C���X�^���V���O�����_���[
	gameObject().lock()->AddComponent<InstancingMeshRenderer>()->MakeOctahedron("StraightMoveEnemy");

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

	// ECS�R���C�_�[
	gameObject().lock()->AddComponent<DeltaCollider>()->SetMain(false);
}

//========================================
//
//	�X�V��
//
//========================================
void StraightMoveEnemyScript::Update()
{
}

//========================================
//
//	��X�V��
//
//========================================
void StraightMoveEnemyScript::LateUpdate()
{
}

//========================================
//
//	�I����
//
//========================================
void StraightMoveEnemyScript::End()
{
}


//******************** �R�[���o�b�N�֐� ********************


//========================================
//
// ����������
//
//========================================
void StraightMoveEnemyScript::OnCollisionEnter(Collider* collider)
{

}

//========================================
//
// �������Ă����
//
//========================================
void StraightMoveEnemyScript::OnCollisionStay(Collider* collider)
{

}

//========================================
//
// ���ꂽ��
//
//========================================
void StraightMoveEnemyScript::OnCollisionExit(Collider* collider)
{

}


//===== ECS =====

//========================================
//
// ����������
//
//========================================
void StraightMoveEnemyScript::OnDeltaCollisionEnter(DeltaCollider* collider)
{
}

//========================================
//
// �������Ă����
//
//========================================
void StraightMoveEnemyScript::OnDeltaCollisionStay(DeltaCollider* collider)
{
}

//========================================
//
// ���ꂽ��
//
//========================================
void StraightMoveEnemyScript::OnDeltaCollisionExit(DeltaCollider* collider)
{
}

