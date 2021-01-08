//==================================================================
//								PlayerScript.cpp
//	�v���C���[�X�N���v�g
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/27	�v���C���[�X�N���v�g�N���X�쐬(Test)
//
//===================================================================


//====== �C���N���[�h�� ======

// ���g
#include "PlayerScript.h"

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
#include "../Engine/ECSCompoent/Rigidbody.h"
#include "../Engine/ECSCompoent/ECSRigidbody.h"
#include "../Engine/ECSCompoent/BoxCollider.h"
#include "../Engine/ECSCompoent/SphereCollider.h"
#include "../Engine/ECSCompoent/ECSSphereCollider.h"

#include "../Engine/Renderer/Camera.h"
#include "../Engine/Renderer/Light.h"

#include "../Engine/ECSSystem/ECSRigidbodySystem.h"
#include "../Engine/ECSSystem/ECSSphereCollisionSystem.h"


// �X�N���v�g
#include "BulletScript.h"


// �l�[���X�y�[�X
using namespace ECS;


//===== �}�N����` =====



//******************** �X�N���v�g���\�b�h ********************


//========================================
//
//	�J�n��
//
//========================================
void PlayerScript::Start()
{
	// ���O�E�^�O
	gameObject().lock()->SetName("Player");
	gameObject().lock()->SetTag("Player");

	transform().lock()->m_pos = Vector3(1000, 1000, 1000);
	transform().lock()->m_scale = Vector3(100, 200, 100);

	// �R���|�[�l���g�̒ǉ�

	// ���W�b�h�{�f�B
	const auto& rb = gameObject().lock()->AddComponent<Rigidbody>();
	m_rb = rb;
	rb->SetMass(2);

	// �����_���[
	//const auto& renderer = gameObject().lock()->AddComponent<MeshRenderer>();
	//renderer->MakeSphere("Player", 100, 50);
	//renderer->MakeCube("Player");
	//renderer->SetDiffuseColor({ 0,1,0,1 });

	// �R���C�_�[
	//const auto& collider = gameObject().lock()->AddComponent<SphereCollider>();
	//collider->SetRadius(50);
	const auto& collider = gameObject().lock()->AddComponent<ECSSphereCollider>();

	// �J����
	CCamera::GetMainCamera()->SetCameraTarget(gameObject().lock()->transform().lock());
	// ���C�g
	CLight::GetMainLight()->SetTargetPos(gameObject().lock()->transform().lock()->m_pos.GetFloat3());
}

//========================================
//
//	�X�V��
//
//========================================
void PlayerScript::Update()
{
	const float speed = 2.0f;
	const float jump = 14.0f;

	Vector3 forward = CCamera::GetMainCamera()->GetForward() * speed;
	forward->y = 0.0f;
	Vector3 right = forward.RotationY(-90);

	// �ړ�
	if (GetKeyPress(VK_W))
	{
		//transform().lock()->m_pos->z += 1.0f;
		m_rb.lock()->AddForce(forward);
	}
	if (GetKeyPress(VK_S))
	{
		//transform().lock()->m_pos->z -= 1.0f;
		m_rb.lock()->AddForce(forward * -1.0f);
	}
	if (GetKeyPress(VK_D))
	{
		//transform().lock()->m_pos->x += 1.0f;
		m_rb.lock()->AddForce(right);
	}
	if (GetKeyPress(VK_A))
	{
		//transform().lock()->m_pos->x -= 1.0f;
		m_rb.lock()->AddForce(right * -1.0f);
	}

	// �W�����v
	if (GetKeyTrigger(VK_SPACE))
	{
		m_rb.lock()->SetForceY(jump);
	}


	// �V���b�g
	m_nShotCnt--;
	if (GetMouseButton(MOUSEBUTTON_L) && m_nShotCnt < 0)
	{
		const auto& test = GetEntityManager()->CreateEntity<GameObject>();
		test->AddComponent<BulletScript>();
		const auto& rb = test->GetComponent<ECSRigidbody>();

		Vector3 dir = CCamera::GetMainCamera()->GetForward().normalized();

		test->transform().lock()->m_pos = transform().lock()->m_pos + dir * 200;
		rb->GetData()->AddForce(dir * 100 + Vector3::WallVerticalVector(m_rb.lock()->GetForce(), dir));
		rb->GetData()->AddTorque(dir * 10);

		m_nShotCnt = 5;
	}
}

//========================================
//
//	��X�V��
//
//========================================
void PlayerScript::LateUpdate()
{
}

//========================================
//
//	�I����
//
//========================================
void PlayerScript::End()
{
}


//******************** �R�[���o�b�N�֐� ********************


//========================================
//
// ����������
//
//========================================
void PlayerScript::OnCollisionEnter(Collider* collider)
{
	// �폜
	GetEntityManager()->DestroyEntity(collider->gameObject().lock());
}

//========================================
//
// �������Ă����
//
//========================================
void PlayerScript::OnCollisionStay(Collider* collider)
{
	// �폜
	GetEntityManager()->DestroyEntity(collider->gameObject().lock());
}

//========================================
//
// ���ꂽ��
//
//========================================
void PlayerScript::OnCollisionExit(Collider* collider)
{
	
}





//========================================
//
// ����������
//
//========================================
void PlayerScript::OnECSCollisionEnter(SphereColliderData* collider)
{
	// �폜
	GetEntityManager()->DestroyEntity(collider->gameObject().lock());
}

//========================================
//
// �������Ă����
//
//========================================
void PlayerScript::OnECSCollisionStay(SphereColliderData* collider)
{
	// �폜
	GetEntityManager()->DestroyEntity(collider->gameObject().lock());
}

//========================================
//
// ���ꂽ��
//
//========================================
void PlayerScript::OnECSCollisionExit(SphereColliderData* collider)
{

}

