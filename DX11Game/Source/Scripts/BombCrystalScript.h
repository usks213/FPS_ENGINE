//==================================================================
//								BombCrystalScript.h
//	�{�������A�C�e�� 
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/10	�{���N���X�^���X�N���v�g�N���X�쐬
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include "../Engine/ECSCompoent/Script.h"


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class BombCrystalScript : public Script
	{
	protected:
		// ----- �����o -----
		int m_nExitTime;	// ��������
		// �X�P�[��
		float m_fScale;
	protected:
		// ----- ���\�b�h -----

		// �J�n���ɌĂ΂�܂�
		virtual void Start() override;
		// ���t���[���Ă΂�܂�
		void Update() override;
		// ���t���[���X�V��ɌĂ΂�܂�
		virtual void LateUpdate() override;
		// �I�����ɌĂ΂�܂�
		virtual void End() override;


		// ----- �R�[���o�b�N�֐� -----

		// Delta
		// ����������
		virtual void OnDeltaCollisionEnter(DeltaCollider* collider) override;
		// �������Ă����
		virtual void OnDeltaCollisionStay(DeltaCollider* collider) override;
		// ���ꂽ��
		void OnDeltaCollisionExit(DeltaCollider* collider) override;
	};
}
