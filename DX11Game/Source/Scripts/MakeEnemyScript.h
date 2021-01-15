//==================================================================
//								MakeEnemyScript.h
//	�G�l�~�[�����X�N���v�g
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/08	�G�l�~�[�����X�N���v�g�N���X�쐬
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include "../Engine/ECSCompoent/Script.h"


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class MakeEnemyScript : public Script
	{
	private:
		// ----- �����o -----
		enum EEnemyType
		{
			eStraightCircle,
			eTracking,
			eGroup,

			eMaxType,
		};

		// �v���C���[�I�u�W�F�N�g
		std::weak_ptr<GameObject> m_player;

		// �����^�C�~���O
		int m_nSpawnInterval;
		// �����J�E���g
		int m_nSpawnCount;
		// ������
		int m_nSpawnNum;
		// �^�C�v
		int m_nType;

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

	public:
		// �v���C���[�̃Z�b�g
		void SetPlayer(std::weak_ptr<GameObject> player) { m_player = player; }
	};
}
