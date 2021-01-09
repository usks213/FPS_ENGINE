//==================================================================
//								TrackingMoveEnemyScript.h
//	�ǔ��^�G�l�~�[
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/08	�g���b�L���O���[�u�G�l�~�[�N���X�쐬
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include "../Engine/ECSCompoent/Script.h"


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class TrackingMoveEnemyScript : public Script
	{
	private:
		// ----- �����o -----
		// ���W�b�h�{�f�B
		std::weak_ptr<Rigidbody> m_rb;

	public:
		// �^�[�Q�b�g�̃Z�b�g
		std::weak_ptr<Rigidbody> GetRb() { return m_rb.lock(); }

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
