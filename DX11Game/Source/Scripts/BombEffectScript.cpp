//==================================================================
//								BombEffectScript.cpp
//	�{���G�t�F�N�g
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/10	�{���G�t�F�N�g�X�N���v�g�N���X�쐬
//
//===================================================================


//====== �C���N���[�h�� ======

// ���g
#include "BombEffectScript.h"

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


// �l�[���X�y�[�X
using namespace ECS;


//===== �}�N����` =====



//******************** �X�N���v�g���\�b�h ********************


//========================================
//
//	�J�n��
//
//========================================
void BombEffectScript::Start()
{
	// ���O�E�^�O
	gameObject().lock()->SetName("BombEffect");
	gameObject().lock()->SetTag("BombEffect");

	// �傫��
	transform().lock()->m_scale = Vector3(20, 20, 20);
	transform().lock()->m_rot->x = 90;
	//--- �R���|�[�l���̒ǉ�

	// �C���X�^���V���O�����_���[
	const auto& rn = gameObject().lock()->AddComponent<InstancingMeshRenderer>();
	rn->MakeOctahedron("BombEffect");
	//rn->SetBillboard(true);
	rn->SetDiffuseColor({ 1,0,1,1 });
	rn->SetBlendStateData(BS_ADDITIVE);

	// ECS���W�b�h�{�f�B
	const auto& rb = gameObject().lock()->AddComponent<Rigidbody>();
	rb->SetDrag({ 0.0f,0.0f,0.0f });
	rb->SetGravityForce({ 0,0,0 });
	rb->SetStaticFriction(0);
	rb->SetDynamicFriction(0);
	rb->SetMass(1);
	rb->SetTorqueDrag({ 0,0,0 });

	// ��������
	m_nExitTime = 180;
}

//========================================
//
//	�X�V��
//
//========================================
void BombEffectScript::Update()
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
void BombEffectScript::LateUpdate()
{
}

//========================================
//
//	�I����
//
//========================================
void BombEffectScript::End()
{
}
