//==================================================================
//								DropDeltaScript.h
//	�G����h���b�v����A�C�e��
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/09	�h���b�v�f���^�X�N���v�g�N���X�쐬
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include "../Engine/ECSCompoent/Script.h"


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class DropDeltaScript : public Script
	{
	private:
		// ----- �����o -----
		int m_nExitTime;	// ��������
		std::weak_ptr<Rigidbody> m_rb;
		// �v���C���[
		std::weak_ptr<GameObject> m_player;
		// �ǔ��t���O
		bool m_IsTracking;
		// �X�P�[��
		float m_fScale;
	public:
		// �v���C���[�̃Z�b�g
		void SetPlayer(std::weak_ptr<GameObject> player) { m_player = player; }

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
