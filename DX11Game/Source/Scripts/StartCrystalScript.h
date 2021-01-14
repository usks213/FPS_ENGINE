//==================================================================
//								StartCrystalScript.h
//	�Q�[���J�n�N���X�^�� 
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/13	�X�^�[�g�N���X�^���X�N���v�g�N���X�쐬
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include "BombCrystalScript.h"


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class StartCrystalScript : public BombCrystalScript
	{
	private:
		// ----- �����o -----
		bool m_bStart;
		std::weak_ptr<GameObject> m_player;

	public:
		// �v���C���[�̃Z�b�g
		void SetPlayer(std::weak_ptr<GameObject> player) { m_player = player; }

	protected:
		// ----- ���\�b�h -----

		// �J�n���ɌĂ΂�܂�
		void Start() override;
		// ���t���[���X�V��ɌĂ΂�܂�
		void LateUpdate() override;
		// �I�����ɌĂ΂�܂�
		void End() override;


		// ----- �R�[���o�b�N�֐� -----

		// Delta
		// ����������
		void OnDeltaCollisionEnter(DeltaCollider* collider) override;
		// �������Ă����
		void OnDeltaCollisionStay(DeltaCollider* collider) override;
	};
}
