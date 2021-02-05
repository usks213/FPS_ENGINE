//==================================================================
//								MakeEnemyScript.h
//	�G�l�~�[�����X�N���v�g
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/08	�G�l�~�[�����X�N���v�g�N���X�쐬
//
//===================================================================


//====== �C���N���[�h�� ======

// ���g
#include "MakeEnemyScript.h"

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
#include "../Engine/ECSCompoent/BillboardRenderer.h"
#include "../Engine/ECSCompoent/AssimpRenderer.h"
#include "../Engine/ECSCompoent/SpriteRenderer.h"
#include "../Engine/ECSCompoent/InstancingMeshRenderer.h"
#include "../Engine/ECSCompoent/Rigidbody.h"
#include "../Engine/ECSCompoent/BoxCollider.h"
#include "../Engine/ECSCompoent/SphereCollider.h"

// ECS�R���|�[�l���g
#include "../Engine/ECSCompoent/Rigidbody.h"
#include "../Engine/ECSCompoent/DeltaCollider.h"

// ECS�V�X�e��
#include "../Engine/ECSSystem/RigidbodySystem.h"
#include "../Engine/ECSSystem/DeltaCollisionSystem.h"

// �X�N���v�g
#include "StraightMoveEnemyScript.h"
#include "GroupMoveEnemyScript.h"
#include "TrackingMoveEnemyHostScript.h"
#include "PlayerScript.h"
#include "BombCrystalScript.h"



// �l�[���X�y�[�X
using namespace ECS;


//===== �}�N����` =====
#define MAX_SPAWN_NUM (50)

#define MAX_ENETETY_NUM (MAX_SPAWN_NUM * 120)


//******************** �X�N���v�g���\�b�h ********************


//========================================
//
//	�J�n��
//
//========================================
void MakeEnemyScript::Start()
{
	// �����̐����^�C�~���O
	m_nSpawnInterval = 60;
	m_nSpawnCount = m_nSpawnInterval;
	// �����̐�����
	m_nSpawnNum = 1;
	// �����^�C�v
	m_nType = 0;
}

//========================================
//
//	�X�V��
//
//========================================
void MakeEnemyScript::Update()
{
	//----- �v���C���[�Ɠ��� -----

	// �v���C���[���擾
	const auto& player = m_player.lock();
	if (!player) return;

	// �v���C���[���W
	const auto& playerTrans = player->transform().lock();
	Vector3 playerPos = player->transform().lock()->m_pos;
	// ���W�̓���
	transform().lock()->m_pos = playerPos;


	//----- �G�l�~�[�̐��� -----

	// �����J�E���g
	m_nSpawnCount--;
	if (m_nSpawnCount >= 0) return;

	// �J�E���g������
	m_nSpawnCount = m_nSpawnInterval;

	// �f���^�����擾
	int nDelta = player->GetComponent<PlayerScript>()->GetDeltaCount();
	for(int i = 1; i < m_nSpawnNum; i++)
		nDelta -= i * 5;
	// ���������v�Z
	if (nDelta / (m_nSpawnNum * 5))
	{
		m_nSpawnNum += 2;
	}
	if (m_nSpawnNum > MAX_SPAWN_NUM) m_nSpawnNum = MAX_SPAWN_NUM;

	// ��������G�l�~�[�̎��
	//int type = rand() % eMaxType;
	//m_nType = (m_nType +1) % 3;
	for (m_nType = 0; m_nType < eMaxType; m_nType++)
	{
		//--- �G�l�~�[�𐶐�������W
		float height = (rand() % 5 - 2) * 100;
		if (playerPos->y < 600) playerPos->y = 100;
		Vector3 spawnPos = { 3000, height, 0 };
		Vector3 temp = playerPos;
		//temp->y = 0;
		spawnPos = spawnPos.RotationY(rand() % 360) + temp;

		//--- �{���N���X�^���̐���
		if (rand() % 100 < 50)
		{
			float bombRand = 1.0f - (rand() % 26 / 100.0f);
			Vector3 bombPos = spawnPos * bombRand;
			bombPos->y = playerPos->y + (7) * 100;
			const auto& bomb = Instantiate<GameObject>(bombPos);
			bomb->AddComponent<BombCrystalScript>();
		}

		// �G���e�B�e�B�̐�
		if (GetEntityManager()->GetEntityCount() > MAX_ENETETY_NUM) continue;

		//--- �^�C�v�ŕ�����
		switch (m_nType)
		{
			// ���i�^�C�v �g��
		case ECS::MakeEnemyScript::eStraightCircle:
		{
			// ������
			int SpawnNum = m_nSpawnNum + 3;

			// �������W
			Vector3 pos = (playerPos - spawnPos).normalized() * SpawnNum * 200;
			pos->y = 0;
			Vector3 backPos = pos;
			// �������̔��������p�x�����炷
			float addAngle = 45.0f / SpawnNum;
			pos = pos.RotationY(-addAngle * (SpawnNum / 2));
			Vector3 center = spawnPos;
			center->y = 0;

			for (int i = 0; i < SpawnNum; i++)
			{
				// �G�l�~�[����
				const auto& obj = Instantiate<GameObject>(pos.RotationY(addAngle * i) - backPos + spawnPos);
				// �R���|�[�l���g�̒ǉ�
				obj->AddComponent<StraightMoveEnemyScript>()->SetPlayer(player);
				// �i�ތ���
				const auto& rb = obj->GetComponent<Rigidbody>();
				Vector3 dir = (temp - center).normalized();
				rb->AddForce(dir * 20);
			}
		}
		break;
		// ���i�^�C�v �^���[
		case ECS::MakeEnemyScript::eGroup:
		{
			// ������
			int SpawnNum = m_nSpawnNum / 2 + 6;

			for (int y = 0; y < SpawnNum / 3; y++)
				for (int x = 0; x < SpawnNum; x += 6)
					for (int i = 0; i < x; i++)
					{
						// �������W
						Vector3 pos = { x * 40.0f, 0, 0 };
						float angle = 360.0f / x * i;
						pos = pos.RotationY(angle);
						Vector3 center = spawnPos;
						center->y += y * 200;
						// �G�l�~�[����
						const auto& obj = Instantiate<GameObject>(pos + center);
						// �R���|�[�l���g�̒ǉ�
						obj->AddComponent<GroupMoveEnemyScript>()->SetPlayer(player);
						// �i�ތ���
						const auto& rb = obj->GetComponent<Rigidbody>();
						center->y = 0;
						Vector3 dir = (temp - center).normalized();
						rb->AddForce(dir * 20);
					}
		}
		break;
		case ECS::MakeEnemyScript::eTracking:
		{
			// �G�l�~�[����
			const auto& obj = Instantiate<GameObject>(spawnPos);
			// �R���|�[�l���g�̒ǉ�
			const auto& tracking = obj->AddComponent<TrackingMoveEnemyHostScript>();
			tracking->SetPlayer(player);

			// ������
			int SpawnNum = m_nSpawnNum * 1.5f + 3;

			// �q�̐���
			tracking->CreateChild(SpawnNum);
		}
		break;
		case ECS::MakeEnemyScript::eMaxType:
			break;
		}
	}
}

//========================================
//
//	��X�V��
//
//========================================
void MakeEnemyScript::LateUpdate()
{
	// �f�o�b�N�\��
	PrintDebugProc("SpawnNum:%d\n\n", m_nSpawnNum);
	PrintDebugProc("EntityNum:%d\n", GetEntityManager()->GetEntityCount());
}

//========================================
//
//	�I����
//
//========================================
void MakeEnemyScript::End()
{
}
