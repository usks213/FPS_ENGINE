//==================================================================
//								GroupMoveEnemyScript.h
//	�O���[�v���[�u�G�l�~�[
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/15	�O���[�v���[�u�G�l�~�[�N���X�쐬
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include "EnemyBaseScript.h"


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class GroupMoveEnemyScript : public EnemyBaseScript
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

	};
}
