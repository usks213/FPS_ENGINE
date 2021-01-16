//==================================================================
//												DeltaDeltaCollisionSystem.cpp
//	�f���^��p�����蔻��Ǘ��V�X�e��
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/09	�f���^�R���W�����V�X�e���쐬
//
//===================================================================


//===== �C���N���[�h�� =====
#include "DeltaCollisionSystem.h"
#include "../ECSCompoent/DeltaCollider.h"
#include "../ECS/Entity/IEntity.h"
#include <algorithm>

#include "../System/CCell.h"
#include "../ECS/Entity/EntityManager.h"

#include "../main.h"
#include "../ECSCompoent/Transform.h"
#include "../Renderer/Camera.h"
#include "../ECSEntity/GameObject.h"

using namespace ECS;


//===== �}�N����` =====

//===== �O���[�o���ϐ� =====



//===================================
//
//	�R���X�g���N�^
//
//===================================
DeltaCollisionSystem::DeltaCollisionSystem(World* pWorld)
	: System<DeltaCollider>(pWorld)
{
	// �X�V��
	SetUpdateOrder(UpdateOrder::eCollision);
}


//===================================
//
//	�f�X�g���N�^
//
//===================================
DeltaCollisionSystem::~DeltaCollisionSystem()
{
}


//===================================
//
//	�������R�[���o�b�N
//
//===================================
void DeltaCollisionSystem::OnCreate()
{
	// �}�b�v�T�C�Y // ���̂���{�ɂȂ��Ă�H�@����4000x4000
	CCell<DeltaCollider>::SetMapSize(100.0f * 5 * 10, 100.0f * 5 * 10);
}

//===================================
//
//	�X�V���R�[���o�b�N
//
//===================================
void DeltaCollisionSystem::OnUpdate()
{
	// ��ԃ��x���̐�
	const unsigned int uiLevel = CCell<DeltaCollider>::GetUnitLevel();
	const unsigned int nMaxCell = MAX_CELL;

	// ��Ԃ̍쐬
	CCell<DeltaCollider> mainMainCell[MAX_CELL];
	CCell<DeltaCollider> mainCell[MAX_CELL];
	CCell<DeltaCollider> subCell[MAX_CELL];
	// ���[�g���ԍ�
	DWORD Def = 0;
	DWORD wLeftTop = 0;
	DWORD wRightDown = 0;
	// �J�������W
	Vector3 cameraPos = CCamera::GetMainCamera()->GetPos();
	// �I�t�Z�b�g
	CCell<DeltaCollider>::SetOffSet(cameraPos->x, cameraPos->z);

	// ��ԕ��� + ��ԍX�V
	std::for_each(m_ComponentList.begin(), m_ComponentList.end(),
		[&](DeltaCollider* collider)
		{
			// ���S���W
			Vector3 boxPos1 = collider->m_transform.lock()->m_pos + collider->m_bound.GetCenter();
			// �{�b�N�X�̍ő�ŏ�
			Vector3 boxMax1 = boxPos1 + Vector3(collider->m_bound.GetMax() * collider->m_transform.lock()->m_scale);
			Vector3 boxMin1 = boxPos1 + Vector3(collider->m_bound.GetMin() * collider->m_transform.lock()->m_scale);

			// �����ŋ�Ԃ̓o�^������
			// ����ƉE�����o��
			wLeftTop = CCell<DeltaCollider>::GetPointElem(boxMin1->x, boxMin1->z);
			wRightDown = CCell<DeltaCollider>::GetPointElem(boxMax1->x, boxMax1->z);
			if (wLeftTop >= nMaxCell - 1 || wRightDown >= nMaxCell - 1)
			{
				// �폜
				GetEntityManager()->DestroyEntity(collider->gameObject().lock());
				return;
			}

			// XOR���Ƃ�	
			Def = wLeftTop ^ wRightDown;
			unsigned int HiLevel = 0;
			unsigned int i;
			for (i = 0; i < uiLevel; i++)
			{
				DWORD Check = (Def >> (i * 2)) & 0x3;
				if (Check != 0)
					HiLevel = i + 1;
			}
			DWORD SpaceNum = wRightDown >> (HiLevel * 2);
			int nPow4 = 1;
			for (i = 0; i < uiLevel - HiLevel; i++) nPow4 *= 4;
			DWORD AddNum = (nPow4 - 1) / 3;
			SpaceNum += AddNum;	// ���ꂪ��������

			// ��ԊO�͂͂���
			if (SpaceNum > nMaxCell - 1)
			{
				// �폜
				GetEntityManager()->DestroyEntity(collider->gameObject().lock());
				return;
			}

			// ���C���R���C�_�[��
			if (collider->m_bMain)
			{
				// �������Ԃ̃��C�����X�g�Ɋi�[
				mainMainCell[SpaceNum].GetList().push_back(collider);
			}
			else
			{
				// �������Ԃ̃��C�����X�g�Ɋi�[
				mainCell[SpaceNum].GetList().push_back(collider);

				// �������Ԃ̐e�̃T�u�Ɋi�[
				while (SpaceNum > 0)
				{
					SpaceNum--;
					SpaceNum /= 4;

					subCell[SpaceNum].GetList().push_back(collider);
				}
			}

			// �ߋ��̏�Ԃ�ۑ�
			collider->m_bOldState = collider->m_bCurState;
			// ���݂̏�Ԃ��X�V
			collider->m_bCurState = false;
		});


	// ���C����Ԃ̓����蔻��
	for (int i = 0; i < nMaxCell; ++i)
	{
		std::for_each(mainMainCell[i].GetList().begin(), mainMainCell[i].GetList().end(),
			[&mainCell, &i](DeltaCollider* collider)
			{
				// �����蔻��
				Collision(collider, mainCell[i].GetList());
			});
	}

	// �T�u��Ԃ̓����蔻��
	for (int i = 0; i < nMaxCell; ++i)
	{
		std::for_each(mainMainCell[i].GetList().begin(), mainMainCell[i].GetList().end(),
			[&subCell, &i](DeltaCollider* collider)
			{
				// �����蔻��
				Collision(collider, subCell[i].GetList());
			});
	}
}

//===================================
//
//	�폜���R�[���o�b�N
//
//===================================
void DeltaCollisionSystem::OnDestroy()
{
}

//========================================
//
//	�m�[�h�ƃ��X�g�̓����蔻��
//
//========================================
void DeltaCollisionSystem::Collision(DeltaCollider* main, const std::list<DeltaCollider*>& m_pSubList)
{
	// �T�u���[�v
	for (const auto& sub : m_pSubList)
	{
		// ����������
		if (main == sub) continue;

		//--- �����蔻�菈�� ---
		if (DeltaCollider::SpheretoSphere(main, sub))
		{
			// ��Ԃ��X�V
			main->m_bCurState = true;
			sub->m_bCurState = true;

			//--- �����蔻��R�[���o�b�N ---
			// Enter
			if (main->m_bCurState && !main->m_bOldState)
			{
				// ���b�Z�[�W���M
				main->m_Parent.lock()->SendComponentMessage("OnDeltaCollisionEnter", sub);
			}
			// Stay
			else if (main->m_bCurState && main->m_bOldState)
			{
				// ���b�Z�[�W���M
				main->m_Parent.lock()->SendComponentMessage("OnDeltaCollisionStay", sub);
			}

			// ���葤
			// Enter
			if (sub->m_bCurState && !sub->m_bOldState)
			{
				// ���b�Z�[�W���M
				sub->m_Parent.lock()->SendComponentMessage("OnDeltaCollisionEnter", main);
			}
			// Stay
			else if (sub->m_bCurState && sub->m_bOldState)
			{
				// ���b�Z�[�W���M
				sub->m_Parent.lock()->SendComponentMessage("OnDeltaCollisionStay", main);
			}
		}

		//--- �����蔻��R�[���o�b�N ---
		// Exit
		if (!main->m_bCurState && main->m_bOldState)
		{
			// ���b�Z�[�W���M
			main->m_Parent.lock()->SendComponentMessage("OnDeltaCollisionExit", sub);
		}
		// Exit
		if (!sub->m_bCurState && sub->m_bOldState)
		{
			// ���b�Z�[�W���M
			sub->m_Parent.lock()->SendComponentMessage("OnDeltaCollisionExit", main);
		}
	}
}