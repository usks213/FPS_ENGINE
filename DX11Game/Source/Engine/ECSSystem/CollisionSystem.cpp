//==================================================================
//												CollisionSystem.cpp
//	�����蔻��Ǘ��V�X�e��
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/27	�R���W�����V�X�e���쐬
//
//===================================================================


//===== �C���N���[�h�� =====
#include "CollisionSystem.h"
#include "../ECSCompoent/Collider.h"
#include "../ECS/Entity/IEntity.h"
#include <algorithm>

#include "../System/CCell.h"

#include "../main.h"
#include "../ECSCompoent/Transform.h"

using namespace ECS;


//===== �}�N����` =====
//#define MAX_CELL (1+4+16+64)
//
//
////===== �N���X��` =====
//// �l���؋��
//class CCell
//{
//public:
//	CCell()
//	{
//
//	}
//	~CCell()
//	{
//		m_list.clear();
//	}
//
//	void ClearList()
//	{
//		m_list.clear();
//	}
//
//	std::list<Collider*>& GetList() { return m_list; }
//
//	static void SetMapSize(float width, float height)
//	{
//		m_fUnit_W = width / (float)(1 << (m_uiLevel - 1));
//		m_fUnit_H = height / (float)(1 << (m_uiLevel - 1));
//	}
//
//	static float GetUnitW() { return m_fUnit_W; }
//	static float GetUnitH() { return m_fUnit_H; }
//	static unsigned int GetUnitLevel() { return m_uiLevel; }
//
//	std::list<Collider*> m_list;
//
//private:
//	static float m_fUnit_W;		// �ŏ����x����Ԃ̕��P��
//	static float m_fUnit_H;		// �ŏ����x����Ԃ̍��P��
//	static const unsigned int m_uiLevel = 3;			// �ŉ��ʃ��x��
//};
//
//// �����T�C�Y
//float CCell::m_fUnit_W = 100.0f;		// �ŏ����x����Ԃ̕��P��
//float CCell::m_fUnit_H = 100.0f;		// �ŏ����x����Ԃ̍��P��
//
//
////===== �v���g�^�C�v�錾 =====
//// �r�b�g�����֐�
//DWORD BitSeparate32(DWORD n)
//{
//	n = (n | (n << 8)) & 0x00ff00ff;
//	n = (n | (n << 4)) & 0x0f0f0f0f;
//	n = (n | (n << 2)) & 0x33333333;
//	return (n | (n << 1)) & 0x55555555;
//}
//
//// 2D���[�g����Ԕԍ��Z�o�֐�
//WORD Get2DMortonNumber(WORD x, WORD y)
//{
//	return (WORD)(BitSeparate32(x) | (BitSeparate32(y) << 1));
//}
//
//// ���W�����`4���ؗv�f�ԍ��ϊ��֐�
//DWORD GetPointElem(float pos_x, float pos_y)
//{
//	// �{���̓t�B�[���h�̑傫���Ƃ�
//	return Get2DMortonNumber((WORD)(pos_x / CCell::GetUnitW()), (WORD)(pos_y / CCell::GetUnitH()));
//}


//===== �O���[�o���ϐ� =====



//===================================
//
//	�R���X�g���N�^
//
//===================================
CollisionSystem::CollisionSystem(World* pWorld)
	: System<Collider>(pWorld)
{
	// �X�V��
	SetUpdateOrder(UpdateOrder::eCollision);
}


//===================================
//
//	�f�X�g���N�^
//
//===================================
CollisionSystem::~CollisionSystem()
{
}


//===================================
//
//	�������R�[���o�b�N
//
//===================================
void CollisionSystem::OnCreate()
{
	// �}�b�v�T�C�Y // ���̂���{�ɂȂ��Ă�H�@����9600x9600
	CCell<Collider>::SetMapSize(100.0f * 8 * 10, 100.0f * 8 * 10);
}

//===================================
//
//	�X�V���R�[���o�b�N
//
//===================================
void CollisionSystem::OnUpdate()
{
	// ��ԃ��x���̐�
	const unsigned int uiLevel = CCell<Collider>::GetUnitLevel();
	const unsigned int nMaxCell = MAX_CELL;

	// ��Ԃ̍쐬
	CCell<Collider> mainCell[MAX_CELL];
	CCell<Collider> subCell[MAX_CELL];
	// ���[�g���ԍ�
	DWORD Def = 0;
	DWORD wLeftTop = 0;
	DWORD wRightDown = 0;


	// ��ԕ��� + ��ԍX�V
	std::for_each(m_ComponentList.begin(), m_ComponentList.end(),
		[&](Collider* collider)
		{
			// ���Ԃ��Ȃ�
			if (!collider) return;

			// ���S���W
			Vector3 boxPos1 = collider->m_transform.lock()->m_pos + collider->m_bound.GetCenter();
			// �{�b�N�X�̍ő�ŏ�
			Vector3 boxMax1 = boxPos1 + Vector3(collider->m_bound.GetMax() * collider->m_transform.lock()->m_scale);
			Vector3 boxMin1 = boxPos1 + Vector3(collider->m_bound.GetMin() * collider->m_transform.lock()->m_scale);

			// �����ŋ�Ԃ̓o�^������
			// ����ƉE�����o��
			wLeftTop = CCell<Collider>::GetPointElem(boxMin1->x, boxMin1->z);
			wRightDown = CCell<Collider>::GetPointElem(boxMax1->x, boxMax1->z);
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

			// �������Ԃ̃��C�����X�g�Ɋi�[
			mainCell[SpaceNum].GetList().push_back(collider);

			// �������Ԃ̐e�̃T�u�Ɋi�[
			while (SpaceNum > 0)
			{
				SpaceNum--;
				SpaceNum /= 4;

				subCell[SpaceNum].GetList().push_back(collider);
			}


			// �ߋ��̏�Ԃ�ۑ�
			collider->m_bOldState = collider->m_bCurState;
			// ���݂̏�Ԃ��X�V
			collider->m_bCurState = false;

			//collider->m_aIsHit.clear();
		});


	// ���C����Ԃ̓����蔻��
	for (int i = 0; i < nMaxCell; ++i)
	{
		std::for_each(mainCell[i].GetList().begin(), mainCell[i].GetList().end(),
			[&mainCell, &i](Collider* collider)
			{
				// �����蔻��
				Collision(collider, mainCell[i].GetList());
			});
	}

	// �T�u��Ԃ̓����蔻��
	for (int i = 0; i < nMaxCell; ++i)
	{
		std::for_each(mainCell[i].GetList().begin(), mainCell[i].GetList().end(),
			[&subCell, &i](Collider* collider)
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
void CollisionSystem::OnDestroy()
{
}

//========================================
//
//	�m�[�h�ƃ��X�g�̓����蔻��
//
//========================================
void CollisionSystem::Collision(Collider* main, const std::list<Collider*>& m_pSubList)
{
	// �T�u���[�v
	for (const auto& sub : m_pSubList)
	{
		// ����������
		if (main == sub) continue;

		//// ���Ɋi�[�ς�
		//auto itr = std::find(main->m_aIsHit.begin(),
		//	main->m_aIsHit.end(), sub);
		//if (itr != main->m_aIsHit.end()) continue;

		//--- �����蔻�菈�� ---
		if (Collider::AABBtoAABB(main, sub))
		{
			// �ڍה���
			if (main->Judgment(sub))
			{
				/*main->m_aIsHit.push_back(sub);
				sub->m_aIsHit.push_back(main);*/


				// ��Ԃ��X�V
				main->m_bCurState = true;
				sub->m_bCurState = true;

				//--- �����蔻��R�[���o�b�N ---
				// Enter
				if (main->m_bCurState && !main->m_bOldState)
				{
					// ���b�Z�[�W���M
					main->m_Parent.lock()->SendComponentMessage("OnCollisionEnter", sub);
				}
				// Stay
				else if (main->m_bCurState && main->m_bOldState)
				{
					// ���b�Z�[�W���M
					main->m_Parent.lock()->SendComponentMessage("OnCollisionStay", sub);
				}

				// ���葤
				// Enter
				if (sub->m_bCurState && !sub->m_bOldState)
				{
					// ���b�Z�[�W���M
					sub->m_Parent.lock()->SendComponentMessage("OnCollisionEnter", main);
				}
				// Stay
				else if (sub->m_bCurState && sub->m_bOldState)
				{
					// ���b�Z�[�W���M
					sub->m_Parent.lock()->SendComponentMessage("OnCollisionStay", main);
				}
			}
		}

		//--- �����蔻��R�[���o�b�N ---
		// Exit
		if (!main->m_bCurState && main->m_bOldState)
		{
			// ���b�Z�[�W���M
			main->m_Parent.lock()->SendComponentMessage("OnCollisionExit", sub);
		}
		// Exit
		if (!sub->m_bCurState && sub->m_bOldState)
		{
			// ���b�Z�[�W���M
			sub->m_Parent.lock()->SendComponentMessage("OnCollisionExit", main);
		}
	}
}