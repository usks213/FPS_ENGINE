//==================================================================
//								ScriptBaseLayout.h
//	�X�N���v�g�x�[�X���C�A�E�g
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/27	�X�N���v�g�x�[�X���C�A�E�g�N���X�쐬
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include "../Engine/ECSCompoent/Script.h"


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class ScriptBaseLayout : public Script
	{
	private:
		// ----- �����o -----

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
		void OnDeltaCollisionEnter(DeltaCollider* collider) override;
		// �������Ă����
		void OnDeltaCollisionStay(DeltaCollider* collider) override;
		// ���ꂽ��
		void OnDeltaCollisionExit(DeltaCollider* collider) override;
	};
}
