//==================================================================
//								TrackingMoveEnemyScript.cpp
//	�ǔ��^�G�l�~�[
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/08	�g���b�L���O���[�u�G�l�~�[�N���X�쐬
//
//===================================================================


//====== �C���N���[�h�� ======

// ���g
#include "TrackingMoveEnemyScript.h"

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
void TrackingMoveEnemyScript::Start()
{
	// ���O�E�^�O
	gameObject().lock()->SetName("TrackingMoveEnemy");
	gameObject().lock()->SetTag("Enemy");

	// �傫��
	transform().lock()->m_scale = Vector3(200, 200, 200);

	//--- �R���|�[�l���̒ǉ�

	// �C���X�^���V���O�����_���[
	gameObject().lock()->AddComponent<InstancingMeshRenderer>()->MakeDodecahedron("TrackingMoveEnemy");

	// ECS���W�b�h�{�f�B
	const auto& rb = gameObject().lock()->AddComponent<ECSRigidbody>();
	rb->GetData()->SetDrag({ 0,0,0 });
	rb->GetData()->SetGravityForce({ 0,0,0 });
	rb->GetData()->SetStaticFriction(0);
	rb->GetData()->SetDynamicFriction(0);
	rb->GetData()->SetMass(1);
	rb->GetData()->SetTorqueDrag({ 0,0,0 });
	// ���W�b�h�{�f�B�ۑ�
	m_rb = rb;

	// ECS�R���C�_�[
	gameObject().lock()->AddComponent<ECSSphereCollider>()->GetData()->SetMain(false);

}

//========================================
//
//	�X�V��
//
//========================================
void TrackingMoveEnemyScript::Update()
{
}

//========================================
//
//	��X�V��
//
//========================================
void TrackingMoveEnemyScript::LateUpdate()
{
}

//========================================
//
//	�I����
//
//========================================
void TrackingMoveEnemyScript::End()
{
}


//******************** �R�[���o�b�N�֐� ********************


//========================================
//
// ����������
//
//========================================
void TrackingMoveEnemyScript::OnCollisionEnter(Collider* collider)
{

}

//========================================
//
// �������Ă����
//
//========================================
void TrackingMoveEnemyScript::OnCollisionStay(Collider* collider)
{

}

//========================================
//
// ���ꂽ��
//
//========================================
void TrackingMoveEnemyScript::OnCollisionExit(Collider* collider)
{

}


//===== ECS =====

//========================================
//
// ����������
//
//========================================
void TrackingMoveEnemyScript::OnECSCollisionEnter(SphereColliderData* collider)
{
}

//========================================
//
// �������Ă����
//
//========================================
void TrackingMoveEnemyScript::OnECSCollisionStay(SphereColliderData* collider)
{
}

//========================================
//
// ���ꂽ��
//
//========================================
void TrackingMoveEnemyScript::OnECSCollisionExit(SphereColliderData* collider)
{
}

