//==================================================================
//								EnemyBaseScript.h
//	�G�l�~�[�x�[�X�X�N���v�g
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/09	�G�l�~�[�x�[�X�N���X�쐬
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include "../Engine/ECSCompoent/Script.h"


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class MakeEnemyScript;

	class EnemyBaseScript : public Script
	{
		friend MakeEnemyScript;
	protected:
		// ----- �����o -----
		// �^�[�Q�b�g
		std::weak_ptr<GameObject> m_player;
		// ���W�b�h�{�f�B
		std::weak_ptr<Rigidbody> m_rb;
		// �ړ����x
		float m_speed;
		// �X�P�[��
		float m_fScale;
		float m_fMaxScale;

	public:
		// ���W�b�h�{�f�B�擾
		std::weak_ptr<Rigidbody> GetRb() { return m_rb; }
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

		// Delta
		// ����������
		virtual void OnDeltaCollisionEnter(DeltaCollider* collider) override;
		// �������Ă����
		virtual void OnDeltaCollisionStay(DeltaCollider* collider) override;
		// ���ꂽ��
		virtual void OnDeltaCollisionExit(DeltaCollider* collider) override;
	};
}
