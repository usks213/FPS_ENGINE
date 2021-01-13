//==================================================================
//								FadeScript.h
//	�t�F�[�h�X�N���v�g
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/13	�t�F�[�h�X�N���v�g�N���X�쐬
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include "../Engine/ECSCompoent/Script.h"
#include <functional>
#include "../Engine/ECSCompoent/SpriteRenderer.h"

//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class FadeScript : public Script
	{
	private:
		// ----- �����o -----
		int m_nAlpha;	// �t�F�[�h�p�|���S���̃A���t�@�l(0�`255)
		int m_nState;	// ���
		int m_nFrame;	// �t�F�[�h�ɂ�����t���[����
		bool m_bNowFade;// �t�F�[�h����

		std::function<void(void)> m_fadeEndFunc;	// �t�F�[�h��̊֐�
		std::weak_ptr<SpriteRenderer> m_render;		// �����_���[
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
		// --- �t�F�[�h

		// �t�F�[�h�A�E�g�̊J�n
		void StartFadeOut(int nFrame = 60);
		// �t�F�[�h����
		bool GetIsFade() { return m_bNowFade; }
		// �t�F�[�h��̊֐��I�u�W�F�N�g��o�^
		void SetFadeEndFunc(std::function<void(void)> func) { m_fadeEndFunc = func; }
	};
}
