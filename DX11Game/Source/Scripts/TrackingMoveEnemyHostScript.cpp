//==================================================================
//								TrackingMoveEnemyHostScript.cpp
//	�ǔ��^�G�l�~�[�𑩂˂�e
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/08	�g���b�L���O���[�u�G�l�~�[�z�X�g�N���X�쐬
//
//===================================================================


//====== �C���N���[�h�� ======

// ���g
#include "TrackingMoveEnemyHostScript.h"

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
void TrackingMoveEnemyHostScript::Start()
{
	// ���O�E�^�O
	gameObject().lock()->SetName("TrackingMoveEnemyHost");
	gameObject().lock()->SetTag("Enemy");

	// �傫��
	transform().lock()->m_scale = Vector3(300, 300, 300);

	//--- �R���|�[�l���̒ǉ�

	// �C���X�^���V���O�����_���[
	gameObject().lock()->AddComponent<InstancingMeshRenderer>()->MakeDodecahedron("TrackingMoveEnemyHost");

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


	// �ړ����x
	m_speed = 18;
}

//========================================
//
//	�X�V��
//
//========================================
void TrackingMoveEnemyHostScript::Update()
{
	// �^�[�Q�b�g�����݂��Ȃ�
	const auto& target = m_target.lock();
	if (!target) return;

	//--- �ǔ�����
	Vector3 dir = target->m_pos - transform().lock()->m_pos;
	dir = dir.normalized();

	// �͂�������
	m_rb.lock()->GetData()->SetForce(dir * m_speed);

	//// �q�ɂ�������
	//for (const auto& child_w : m_childList)
	//{
	//	const auto& child_s = child_w.lock();
	//	if (!child_s) continue;

	//	// �͂�������
	//	child_s->GetRb()->SetForce(dir * m_speed);
	//}
}

//========================================
//
//	��X�V��
//
//========================================
void TrackingMoveEnemyHostScript::LateUpdate()
{
}

//========================================
//
//	�I����
//
//========================================
void TrackingMoveEnemyHostScript::End()
{
}

//========================================
//
//	�q�̐���
//
//========================================
void TrackingMoveEnemyHostScript::CreateChild(int nNum)
{
	//x = Mathf.Cos(phi) * Mathf.Cos(theta);
	//y = Mathf.Cos(phi) * Mathf.Sin(theta);
	//z = Mathf.Sin(phi);
	for (int n = 0; n < nNum; ++n)
	{
		for (int i = 0; i < n; ++i)
		{
			float phi = 360.0f / i;

			for (int j = 0; j < n; ++j)
			{
				float theta = 360.0f / j;
				// ���W
				Vector3 pos = {
					CosDeg(phi) * CosDeg(theta),
					CosDeg(phi) * SinDeg(theta),
					SinDeg(phi)
				};
				pos *= 200;

				// �G�l�~�[����
				const auto& obj = Instantiate<GameObject>(pos);
				// �R���|�[�l���g�̒ǉ�
				const auto& tracking = obj->AddComponent<TrackingMoveEnemyScript>();
				// ���X�g�֊i�[
				//m_childList.push_back(tracking);
			}
		}
	}
}


//******************** �R�[���o�b�N�֐� ********************


//========================================
//
// ����������
//
//========================================
void TrackingMoveEnemyHostScript::OnCollisionEnter(Collider* collider)
{

}

//========================================
//
// �������Ă����
//
//========================================
void TrackingMoveEnemyHostScript::OnCollisionStay(Collider* collider)
{

}

//========================================
//
// ���ꂽ��
//
//========================================
void TrackingMoveEnemyHostScript::OnCollisionExit(Collider* collider)
{

}


//===== ECS =====

//========================================
//
// ����������
//
//========================================
void TrackingMoveEnemyHostScript::OnECSCollisionEnter(SphereColliderData* collider)
{
}

//========================================
//
// �������Ă����
//
//========================================
void TrackingMoveEnemyHostScript::OnECSCollisionStay(SphereColliderData* collider)
{
}

//========================================
//
// ���ꂽ��
//
//========================================
void TrackingMoveEnemyHostScript::OnECSCollisionExit(SphereColliderData* collider)
{
}

