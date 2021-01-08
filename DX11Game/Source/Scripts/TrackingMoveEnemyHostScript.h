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
#include "../Engine/ECSCompoent/Script.h"

#include "../Engine/ECSCompoent/ECSRigidbody.h"
#include "TrackingMoveEnemyScript.h"

//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class TrackingMoveEnemyHostScript : public Script
	{
	private:
		// ----- �����o -----

		// �^�[�Q�b�g
		std::weak_ptr<Transform> m_target;
		// ���W�b�h�{�f�B
		std::weak_ptr<ECSRigidbody> m_rb;
		// �ړ����x
		float m_speed;

		// �q���X�g
		std::list<std::weak_ptr<TrackingMoveEnemyScript>> m_childList;

	public:
		// �^�[�Q�b�g�̃Z�b�g
		void SetTarget(std::weak_ptr<Transform> target) { m_target = target; }
		// �q�̐���
		void CreateChild(int nNum);

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
