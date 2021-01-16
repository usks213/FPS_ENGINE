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
#include "../Engine/System/debugproc.h"

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
#include "../Engine/ECSCompoent/DeltaCollider.h"

// �����_���[
#include "../Engine/Renderer/Camera.h"
#include "../Engine/Renderer/Light.h"
#include "../Engine/Renderer/PostProcessing.h"

#include "../Engine/ECSSystem/DeltaCollisionSystem.h"

// ���[���h
#include "../Engine/ECS/World/WorldManager.h"
#include "../Engine/ECSWorld/TitleWorld.h"

// �X�N���v�g
#include "BulletScript.h"
#include "DropDeltaScript.h"
#include "GameOverScript.h"


#include <iostream>


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

	transform().lock()->m_pos = Vector3(0, 1000, 0);
	transform().lock()->m_scale = Vector3(300, 600, 300);

	// �R���|�[�l���g�̒ǉ�

	// ���W�b�h�{�f�B
	const auto& rb = gameObject().lock()->AddComponent<Rigidbody>();
	m_rb = rb;
	rb->SetMass(2);

	//// �����_���[
	//const auto& renderer = gameObject().lock()->AddComponent<MeshRenderer>();
	//renderer->MakeSphere("Player", 100, 50);
	//renderer->SetDiffuseColor({ 0,1,0,1 });

	// �R���C�_�[
	//const auto& collider = gameObject().lock()->AddComponent<SphereCollider>();
	//collider->SetRadius(50);
	const auto& collider = gameObject().lock()->AddComponent<DeltaCollider>();

	// �J����
	CCamera::GetMainCamera()->SetCameraTarget(gameObject().lock()->transform().lock());
	// ���C�g
	CLight::GetMainLight()->SetLightTarget(gameObject().lock()->transform().lock());


	// �W�����v
	m_nJump = 0;
	// �f���^�J�E���^�[
	m_nDeltaCount = 0;
	// �V���b�g
	m_bShot = false;

	//HP
	m_fHP = m_fMaxHP;

	// �q�[��
	m_fHeel = 5.0f / 60.0f;
	m_nHeelCnt = m_nHeelInteral;

	// �_���[�W
	m_fDamage = 30.0f;
	m_nDamageCnt = m_nDamageInteral;

	// �A�N�e�B�u
	m_bActive = true;
}

//========================================
//
//	�X�V��
//
//========================================
void PlayerScript::Update()
{
	// �A�N�e�B�u
	if (!m_bActive) return;

	const float speed = 1.0f;
	const float jump = 20.0f;

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
	if (GetKeyTrigger(VK_SPACE) && 
		(transform().lock()->m_pos->y <= transform().lock()->m_scale->y / 2 || m_nJump > 0))
	{
		m_rb.lock()->SetForceY(jump + m_nJump);
		// �T�E���h
		CSound::PlaySE("Jump.wav", 0.7f);
		// ��ʗh��
		CCamera::GetMainCamera()->SetShakeFrame(8);
		m_bGround = false;
	}
	m_nJump--;
	if (m_nJump < 0) m_nJump = 0;

	// �V���b�g
	m_nShotCnt--;
	if (m_bShot && m_nShotCnt < 0)
	{
		//const auto& test = GetEntityManager()->CreateEntity<GameObject>();
		const auto& test = Instantiate<GameObject>();
		test->AddComponent<BulletScript>();
		const auto& rb = test->GetComponent<Rigidbody>();

		Vector3 dir = CCamera::GetMainCamera()->GetForward().normalized();

		test->transform().lock()->m_pos = transform().lock()->m_pos + dir * 500;
		rb->AddForce(dir * 100 + Vector3::WallVerticalVector(m_rb.lock()->GetForce(), dir));
		rb->AddTorque(dir * 10);

		m_nShotCnt = 5;

		// �T�E���h
		CSound::PlaySE("Shot.wav", 0.12f);

	}
}

//========================================
//
//	��X�V��
//
//========================================
void PlayerScript::LateUpdate()
{
	// �A�N�e�B�u
	if (!m_bActive) return;

	// �f�o�b�N�\��
	PrintDebugProc("DeltaCount:%d\n", m_nDeltaCount);

	if (!m_bGround && transform().lock()->m_pos->y <= transform().lock()->m_scale->y / 2)
	{
		m_bGround = true;
		// ��ʗh��
		CCamera::GetMainCamera()->SetShakeFrame(6);
		// �T�E���h
		CSound::PlaySE("PlayerGround.wav", 1.0f);
	}


	// �X�e�[�^�X
	m_nHeelCnt--;
	if (m_nHeelCnt < 0)
	{
		// ��
		m_fHP += m_fHeel;
	}
	// �ő�l
	if (m_fHP > m_fMaxHP)
		m_fHP = m_fMaxHP;

	// �_���[�W
	m_nDamageCnt--;


	// HP����ʂɔ��f
	auto post = PostProcessing::GetMain();
	post->GetColor()->y = m_fHP / m_fMaxHP;
	post->GetColor()->z = m_fHP / m_fMaxHP;
}

//========================================
//
//	�I����
//
//========================================
void PlayerScript::End()
{
	FILE* fp;
	int nScore = 0;

	// �O��̃X�R�A��ǂݍ���
	fopen_s(&fp, "data/score.bin", "rb");

	if (fp)
	{
		fread(&nScore, sizeof(int), 1, fp);

		fclose(fp);
	}

	if (m_nDeltaCount > nScore)
	{
		// �X�R�A�̏����o��
		fopen_s(&fp, "data/score.bin", "wb");

		if (fp)
		{
			fwrite(&m_nDeltaCount, sizeof(int), 1, fp);

			fclose(fp);
		}
	}
}


//******************** �R�[���o�b�N�֐� ********************

//========================================
//
// ����������
//
//========================================
void PlayerScript::OnDeltaCollisionEnter(DeltaCollider* collider)
{
	// �A�N�e�B�u
	if (!m_bActive) return;

	if (collider->gameObject().lock()->tag() == "DropDelta")
	{
		// �J�E���^�[���Z
		m_nDeltaCount++;
	}
	else if (collider->gameObject().lock()->tag() == "BombCrystal")
	{
		m_nJump = 15;
		// ��ʗh��
		CCamera::GetMainCamera()->SetShakeFrame(6);
		// ��
		m_fHP += m_fHeel * 60.0f;
	}
	else if (collider->gameObject().lock()->tag() == "StartCrystal")
	{
		m_bShot = true;
		// ��ʗh��
		CCamera::GetMainCamera()->SetShakeFrame(6);
		// BGM
		CSound::PlayBGM("GameBGM.mp3", 0.3f);

	}
	else if (collider->gameObject().lock()->tag() == "Enemy")
	{
		// ���������p�x���v�Z
		Vector3 vec = collider->transform().lock()->m_pos - transform().lock()->m_pos;
		Vector3 forward = CCamera::GetMainCamera()->GetForward();

		if (Vector3::Dot(vec.normalized(), forward.normalized()) < -0.3f)
		{
			// �_���[�W
			if (m_nDamageCnt > 0) return;
			m_nDamageCnt = m_nDamageInteral;

			// HP
			m_fHP -= m_fDamage;
			m_nHeelCnt = m_nHeelInteral;

			// ��ʗh��
			CCamera::GetMainCamera()->SetShakeFrame(16);

			// �T�E���h
			CSound::PlaySE("PlayerDamage.wav", 1.0f);

			if (m_fHP > 0) return;
			// �Q�[���I�[�o�[
			const auto& gameover = GetEntityManager()->CreateEntity<GameObject>();
			gameover->AddComponent<GameOverScript>();

			// �A�N�e�B�u
			m_bActive = false;

			// �폜
			Destroy(gameObject().lock());
		}
	}
}

//========================================
//
// �������Ă����
//
//========================================
void PlayerScript::OnDeltaCollisionStay(DeltaCollider* collider)
{
	// ��������
	OnDeltaCollisionEnter(collider);
}

//========================================
//
// ���ꂽ��
//
//========================================
void PlayerScript::OnDeltaCollisionExit(DeltaCollider* collider)
{

}

