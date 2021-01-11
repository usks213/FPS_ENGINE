//==================================================================
//								BombEffectScript.h
//	�{���G�t�F�N�g
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/10	�{���G�t�F�N�g�X�N���v�g�N���X�쐬
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include "../Engine/ECSCompoent/Script.h"


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class BombEffectScript : public Script
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

	};
}
