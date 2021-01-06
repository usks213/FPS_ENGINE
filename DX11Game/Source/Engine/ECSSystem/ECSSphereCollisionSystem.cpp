//==================================================================
//												ECSSphereCollisionSystem.cpp
//	�f�[�^�w���ŋ������蔻��Ǘ��V�X�e��
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/01/06	�f�[�^�w���łō쐬
//
//===================================================================


//===== �C���N���[�h�� =====
#include "ECSSphereCollisionSystem.h"
#include "../ECSCompoent/ECSSphereCollider.h"
#include <algorithm>
#include "../main.h"
#include "../ECSCompoent/Transform.h"
#include "../ECS/Entity/IEntity.h"
#include "../System/CCell.h"

#include "../Renderer/Camera.h"

using namespace ECS;


//===== �}�N����` =====



//===================================
//
//	�R���X�g���N�^
//
//===================================
ECSSphereCollisionSystem::ECSSphereCollisionSystem(World* pWorld)
	: ECSSystem<SphereColliderData>(pWorld)
{
	// �X�V��
	SetUpdateOrder(UpdateOrder::eCollision);
}


//===================================
//
//	�f�X�g���N�^
//
//===================================
ECSSphereCollisionSystem::~ECSSphereCollisionSystem()
{
}


//===================================
//
//	�������R�[���o�b�N
//
//===================================
void ECSSphereCollisionSystem::OnCreate()
{
	// �}�b�v�T�C�Y // ���̂���{�ɂȂ��Ă�H�@����9600x9600
	CCell<SphereColliderData>::SetMapSize(100.0f * 8 * 10, 100.0f * 8 * 10);
}

//===================================
//
//	�X�V���R�[���o�b�N
//
//===================================
void ECSSphereCollisionSystem::OnUpdate()
{
	// �|�C���^�̍X�V
	ReLoadComponentData();

	// ��ԃ��x���̐�
	const unsigned int uiLevel = CCell<SphereColliderData>::GetUnitLevel();
	const unsigned int nMaxCell = MAX_CELL;

	// ��Ԃ̍쐬
	CCell<SphereColliderData> mainMainCell[MAX_CELL];
	CCell<SphereColliderData> mainCell[MAX_CELL];
	CCell<SphereColliderData> subCell[MAX_CELL];
	// ���[�g���ԍ�
	DWORD Def = 0;
	DWORD wLeftTop = 0;
	DWORD wRightDown = 0;
	// �J�������W
	Vector3 cameraPos = CCamera::GetMainCamera()->GetPos();
	// �I�t�Z�b�g
	CCell<SphereColliderData>::SetOffSet(cameraPos->x, cameraPos->z);

	// ��ԕ��� + ��ԍX�V
	std::for_each(m_ComponentList.begin(), m_ComponentList.end(),
		[&](SphereColliderData& collider)
		{
			// ���S���W
			Vector3 boxPos1 = collider.m_transform.lock()->m_pos + collider.m_bound.GetCenter();
			// �{�b�N�X�̍ő�ŏ�
			Vector3 boxMax1 = boxPos1 + Vector3(collider.m_bound.GetMax() * collider.m_transform.lock()->m_scale);
			Vector3 boxMin1 = boxPos1 + Vector3(collider.m_bound.GetMin() * collider.m_transform.lock()->m_scale);

			// �����ŋ�Ԃ̓o�^������
			// ����ƉE�����o��
			wLeftTop = CCell<SphereColliderData>::GetPointElem(boxMin1->x, boxMin1->z);
			wRightDown = CCell<SphereColliderData>::GetPointElem(boxMax1->x, boxMax1->z);
			if (wLeftTop >= nMaxCell - 1 || wRightDown >= nMaxCell - 1) return;

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
			if (SpaceNum > nMaxCell - 1) return;

			// ���C���R���C�_�[��
			if (collider.m_bMain)
			{
				// �������Ԃ̃��C�����X�g�Ɋi�[
				mainMainCell[SpaceNum].GetList().push_back(&collider);
			}
			else
			{
				// �������Ԃ̃��C�����X�g�Ɋi�[
				mainCell[SpaceNum].GetList().push_back(&collider);

				// �������Ԃ̐e�̃T�u�Ɋi�[
				while (SpaceNum > 0)
				{
					SpaceNum--;
					SpaceNum /= 4;

					subCell[SpaceNum].GetList().push_back(&collider);
				}
			}

			// �ߋ��̏�Ԃ�ۑ�
			collider.m_bOldState = collider.m_bCurState;
			// ���݂̏�Ԃ��X�V
			collider.m_bCurState = false;
		});


	// ���C����Ԃ̓����蔻��
	for (int i = 0; i < nMaxCell; ++i)
	{
		std::for_each(mainMainCell[i].GetList().begin(), mainMainCell[i].GetList().end(),
			[&mainCell, &i](SphereColliderData* collider)
			{
				// �����蔻��
				Collision(collider, mainCell[i].GetList());
			});
	}

	// �T�u��Ԃ̓����蔻��
	for (int i = 0; i < nMaxCell; ++i)
	{
		std::for_each(mainMainCell[i].GetList().begin(), mainMainCell[i].GetList().end(),
			[&subCell, &i](SphereColliderData* collider)
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
void ECSSphereCollisionSystem::OnDestroy()
{
}

//========================================
//
//	�m�[�h�ƃ��X�g�̓����蔻��
//
//========================================
void ECSSphereCollisionSystem::Collision(SphereColliderData* main, const std::list<SphereColliderData*>& m_pSubList)
{
	// �T�u���[�v
	for (const auto& sub : m_pSubList)
	{
		// ����������
		if (main == sub) continue;

		//--- �����蔻�菈�� ---
		if (SphereColliderData::SpheretoSphere(main, sub))
		{
			// ��Ԃ��X�V
			main->m_bCurState = true;
			sub->m_bCurState = true;

			//--- �����蔻��R�[���o�b�N ---
			// Enter
			if (main->m_bCurState && !main->m_bOldState)
			{
				// ���b�Z�[�W���M
				main->m_Parent.lock()->SendComponentMessage("OnECSCollisionEnter", sub);
			}
			// Stay
			else if (main->m_bCurState && main->m_bOldState)
			{
				// ���b�Z�[�W���M
				main->m_Parent.lock()->SendComponentMessage("OnECSCollisionStay", sub);
			}

			// ���葤
			// Enter
			if (sub->m_bCurState && !sub->m_bOldState)
			{
				// ���b�Z�[�W���M
				sub->m_Parent.lock()->SendComponentMessage("OnECSCollisionEnter", main);
			}
			// Stay
			else if (sub->m_bCurState && sub->m_bOldState)
			{
				// ���b�Z�[�W���M
				sub->m_Parent.lock()->SendComponentMessage("OnECSCollisionStay", main);
			}
		}

		//--- �����蔻��R�[���o�b�N ---
		// Exit
		if (!main->m_bCurState && main->m_bOldState)
		{
			// ���b�Z�[�W���M
			main->m_Parent.lock()->SendComponentMessage("OnECSCollisionExit", sub);
		}
		// Exit
		if (!sub->m_bCurState && sub->m_bOldState)
		{
			// ���b�Z�[�W���M
			sub->m_Parent.lock()->SendComponentMessage("OnECSCollisionExit", main);
		}
	}
}