//==================================================================
//								TrackingMoveEnemyHostScript.h
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

#pragma once

//====== �C���N���[�h�� ======
#include "EnemyBaseScript.h"

#include "TrackingMoveEnemyScript.h"

//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class TrackingMoveEnemyHostScript : public EnemyBaseScript
	{
	private:
		// ----- �����o -----
		// �q���X�g
		std::list<std::weak_ptr<TrackingMoveEnemyScript>> m_childList;
		// ��������
		int m_nExitTime;	
	protected:
		// ----- ���\�b�h -----

		// �J�n���ɌĂ΂�܂�
		void Start() override;
		// ���t���[���Ă΂�܂�
		void Update() override;
		// ���t���[���X�V��ɌĂ΂�܂�
		void LateUpdate() override;
		// �I�����ɌĂ΂�܂�
		void End() override;

		// ----- �R�[���o�b�N�֐� -----

		// ����������
		void OnDeltaCollisionEnter(DeltaCollider* collider) override;
		// �������Ă����
		void OnDeltaCollisionStay(DeltaCollider* collider) override;
	public:
		// �q�̐���
		void CreateChild(int nNum);
	};
}
