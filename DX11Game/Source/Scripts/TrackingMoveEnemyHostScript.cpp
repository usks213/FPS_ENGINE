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
void TrackingMoveEnemyHostScript::Start()
{
	// ���O�E�^�O
	gameObject().lock()->SetName("TrackingMoveEnemyHost");
	gameObject().lock()->SetTag("Enemy");

	// �傫��
	transform().lock()->m_scale = Vector3(100, 100, 100);

	//--- �R���|�[�l���̒ǉ�

	// �C���X�^���V���O�����_���[
	gameObject().lock()->AddComponent<InstancingMeshRenderer>()->MakeDodecahedron("TrackingMoveEnemyHost");

	// ECS���W�b�h�{�f�B
	const auto& rb = gameObject().lock()->AddComponent<Rigidbody>();
	rb->SetDrag({ 0,0,0 });
	rb->SetGravityForce({ 0,0,0 });
	rb->SetStaticFriction(0);
	rb->SetDynamicFriction(0);
	rb->SetMass(1);
	rb->SetTorqueDrag({ 0,0,0 });
	// ���W�b�h�{�f�B�ۑ�
	m_rb = rb;
	// ��]
	Vector3 v = { rand() % 100 / 100.0f, rand() % 100 / 100.0f, rand() % 100 / 100.0f };
	v = v.normalized();
	rb->AddTorque(v * 3);

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
void TrackingMoveEnemyHostScript::Update()
{
	// �^�[�Q�b�g�����݂��Ȃ�
	const auto& target = m_target.lock();
	if (!target) return;

	//--- �ǔ�����
	Vector3 dir = target->m_pos - transform().lock()->m_pos;
	dir = dir.normalized();

	// �͂�������
	m_rb.lock()->SetForce(dir * m_speed);

	// �q�ɂ�������
	for (const auto& child_w : m_childList)
	{
		const auto& child_s = child_w.lock();
		if (!child_s) continue;

		// �͂�������
		child_s->GetRb().lock()->SetForce(dir * m_speed);
	}
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
	//for (int n = 0; n < nNum; ++n)
	//{
	//	for (int i = 0; i < n; ++i)
	//	{
	//		float phi = 360.0f / n * i * (XM_PI / 180.0f);

	//		for (int j = 0; j < n; ++j)
	//		{
	//			float theta = 360.0f / n * j * (XM_PI / 180.0f);
	//			// ���W
	//			Vector3 pos;
	//			pos->x = cosf(phi) * cosf(theta);
	//			pos->y = cosf(phi) * sinf(theta);
	//			pos->z = sinf(phi);
	//			pos *= 100 * n;

	//			// �G�l�~�[����
	//			const auto& obj = Instantiate<GameObject>(pos + transform().lock()->m_pos);
	//			// �R���|�[�l���g�̒ǉ�
	//			const auto& tracking = obj->AddComponent<TrackingMoveEnemyScript>();
	//			// ���X�g�֊i�[
	//			m_childList.push_back(tracking);
	//		}
	//	}
	//}

	for (int n = 0; n < nNum; n++)
	{
		float phi = rand() % 1000 / 100.0f;

		float theta = rand() % 1000 / 100.0f;
		// ���W
		Vector3 pos;
		pos->x = cosf(phi) * cosf(theta);
		pos->y = sinf(phi);
		pos->z = cosf(phi) * sinf(theta);
		pos->x *= (150 + (n / (n * ((n / 8) + 1) + 1)) * 100);
		pos->z *= (150 + (n / (n * ((n / 8) + 1) + 1)) * 100);
		pos->y *= (150 + (n / (n * ((n / 4) + 1) + 1)) * 100);

		// �G�l�~�[����
		const auto& obj = Instantiate<GameObject>(pos + transform().lock()->m_pos);
		// �R���|�[�l���g�̒ǉ�
		const auto& tracking = obj->AddComponent<TrackingMoveEnemyScript>();
		// ���X�g�֊i�[
		m_childList.push_back(tracking);
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
void TrackingMoveEnemyHostScript::OnDeltaCollisionEnter(DeltaCollider* collider)
{
}

//========================================
//
// �������Ă����
//
//========================================
void TrackingMoveEnemyHostScript::OnDeltaCollisionStay(DeltaCollider* collider)
{
}

//========================================
//
// ���ꂽ��
//
//========================================
void TrackingMoveEnemyHostScript::OnDeltaCollisionExit(DeltaCollider* collider)
{
}

