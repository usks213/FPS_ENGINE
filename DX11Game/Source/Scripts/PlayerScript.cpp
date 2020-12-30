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
#include "../Engine/ECSCompoent/BoxCollider.h"
#include "../Engine/ECSCompoent/SphereCollider.h"

#include "../Engine/Renderer/Camera.h"

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
	transform().lock()->m_pos = Vector3(100, 400, 200);
	transform().lock()->m_scale = Vector3(100, 100, 100);

	// �R���|�[�l���g�̒ǉ�

	// ���W�b�h�{�f�B
	const auto& rb = gameObject().lock()->AddComponent<Rigidbody>();
	m_rb = rb;
	rb->SetMass(2);

	// �����_���[
	const auto& renderer = gameObject().lock()->AddComponent<MeshRenderer>();
	//renderer->MakeSphere("Player", 100, 50);
	renderer->MakeCube("Player");
	renderer->SetDiffuseColor({ 0,1,0,1 });

	// �R���C�_�[
	//const auto& collider = gameObject().lock()->AddComponent<SphereCollider>();
	//collider->SetRadius(50);
	const auto& collider = gameObject().lock()->AddComponent<BoxCollider>();

	// �J����
	CCamera::GetMainCamera()->SetCameraTarget(gameObject().lock()->transform().lock());
}

//========================================
//
//	�X�V��
//
//========================================
void PlayerScript::Update()
{
	if (GetKeyPress(VK_UP))
	{
		//transform().lock()->m_pos->z += 1.0f;
		m_rb.lock()->AddForceZ(2.0f);
	}
	if (GetKeyPress(VK_DOWN))
	{
		//transform().lock()->m_pos->z -= 1.0f;
		m_rb.lock()->AddForceZ(-2.0f);
	}
	if (GetKeyPress(VK_RIGHT))
	{
		//transform().lock()->m_pos->x += 1.0f;
		m_rb.lock()->AddForceX(2.0f);
	}
	if (GetKeyPress(VK_LEFT))
	{
		//transform().lock()->m_pos->x -= 1.0f;
		m_rb.lock()->AddForceX(-2.0f);
	}

	if (GetKeyTrigger(VK_SPACE))
	{
		m_rb.lock()->SetForceY(8.0f);
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
	//GetEntityManager()->DestroyEntity(collider->gameObject().lock());
}

//========================================
//
// �������Ă����
//
//========================================
void PlayerScript::OnCollisionStay(Collider* collider)
{
	// �F�ύX
	collider->gameObject().lock()->GetComponent<MeshRenderer>()
		->SetDiffuseColor({ 1,0,0,1 });
}

//========================================
//
// ���ꂽ��
//
//========================================
void PlayerScript::OnCollisionExit(Collider* collider)
{
	// �F�ύX
	collider->gameObject().lock()->GetComponent<MeshRenderer>()
		->SetDiffuseColor({ 1,1,1,1 });
}

