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
		void OnCollisionEnter(Collider* collider) override;
		// �������Ă����
		void OnCollisionStay (Collider* collider) override;
		// ���ꂽ��
		void OnCollisionExit (Collider* collider) override;

		// ECS
		// ����������
		void OnECSCollisionEnter(SphereColliderData* collider) override;
		// �������Ă����
		void OnECSCollisionStay(SphereColliderData* collider) override;
		// ���ꂽ��
		void OnECSCollisionExit(SphereColliderData* collider) override;
	};
}
