//==================================================================
//								PlayerScript.h
//	�v���C���[�X�N���v�g
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/27	�v���C���[�X�N���v�g�N���X�쐬(Test)
//	2021/01/09	�f���^�J�E���^�[�̒ǉ�
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include "../Engine/ECSCompoent/Script.h"


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class PlayerScript : public Script
	{
	private:
		// ----- �����o -----
		std::weak_ptr<Rigidbody> m_rb;
		int m_nShotCnt;
		// �W�����v�t���O
		int m_nJump;
		// �f���^�J�E���^�[
		int m_nDeltaCount;
		// �V���b�g
		bool m_bShot;
	public:
		// �f���^���擾
		int GetDeltaCount() { return m_nDeltaCount; }

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
		// ���ꂽ��
		void OnDeltaCollisionExit(DeltaCollider* collider) override;
	};
}
