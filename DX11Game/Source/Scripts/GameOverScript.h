//==================================================================
//								GameOverScript.h
//	�Q�[���I�[�o�[���o
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/15	�Q�[���I�[�o�[�N���X�쐬
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include "../Engine/ECSCompoent/Script.h"
#include "../Engine/ECSCompoent/SpriteRenderer.h"

//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class GameOverScript : public Script
	{
	private:
		// ----- �����o -----
		// �X�v���C�g
		std::weak_ptr<SpriteRenderer> m_sprite;
		// �A���t�@
		float m_fAlpha;
		float m_fAddAlpha;

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
