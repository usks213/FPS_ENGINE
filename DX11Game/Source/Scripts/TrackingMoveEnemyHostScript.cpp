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
	// �x�[�X�N���X
	EnemyBaseScript::Start();

	// ���O�E�^�O
	gameObject().lock()->SetName("TrackingMoveEnemyHost");
	gameObject().lock()->SetTag("Host");

	// �傫��
	m_fMaxScale = 100;

	//--- �R���|�[�l���̒ǉ�

	// �C���X�^���V���O�����_���[
	//gameObject().lock()->AddComponent<InstancingMeshRenderer>()->MakeDodecahedron("TrackingMoveEnemy");

	// ��������
	m_nExitTime = 360;
}

//========================================
//
//	�X�V��
//
//========================================
void TrackingMoveEnemyHostScript::Update()
{
	// �x�[�X�N���X
	EnemyBaseScript::Update();

	// ��������
	m_nExitTime--;
	if (m_nExitTime < 0)
	{
		// ���g�̍폜
		GetEntityManager()->DestroyEntity(gameObject().lock());
	}

	// �^�[�Q�b�g�����݂��Ȃ�
	const auto& player = m_player.lock();
	if (!player) return;
	const auto& target = player->transform().lock();

	//--- �ǔ�����
	Vector3 dir = target->m_pos - transform().lock()->m_pos;
	if (dir.magnitude() < 500) return;
	dir = dir.normalized();

	// �����Ă����
	Vector3 cameraDir = CCamera::GetMainCamera()->GetForward();
	if (Vector3::Dot(cameraDir, dir) >= 0) return;

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
	// �x�[�X�N���X
	EnemyBaseScript::LateUpdate();
}

//========================================
//
//	�I����
//
//========================================
void TrackingMoveEnemyHostScript::End()
{
	// �x�[�X�N���X
	EnemyBaseScript::End();
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

	int j = 0;
	int k = 0;
	for (int n = 0; n < nNum; n++)
	{
		float phi = rand() % 1000 / 100.0f;

		float theta = rand() % 1000 / 100.0f;
		// ���W
		Vector3 pos;
		pos->x = cosf(phi) * cosf(theta);
		pos->y = sinf(phi);
		pos->z = cosf(phi) * sinf(theta);

		if (n > j * 4)
		{
			k++;
			j += k;
		}
		pos->x *= (150 + k * 150);
		pos->z *= (150 + k * 150);
		pos->y *= (150 + k * 2 * 150);

		// �G�l�~�[����
		const auto& obj = Instantiate<GameObject>(pos + transform().lock()->m_pos);
		// �R���|�[�l���g�̒ǉ�
		const auto& tracking = obj->AddComponent<TrackingMoveEnemyScript>();
		tracking->SetPlayer(m_player.lock());
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
