//==================================================================
//								TitleScoreUIScript.h
//	�^�C�g���ł̃����\��UI
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/15	�^�C�g�����\��UI�N���X�쐬
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
	class TitleScoreUIScript;

	class TitleScoreUIData :public Script
	{
		friend TitleScoreUIScript;
	private:
		// ----- �����o -----
		std::weak_ptr<SpriteRenderer> m_sprite;
		int m_nAnimNo;

	public:
		// ----- ���\�b�h -----

		// �J�n���ɌĂ΂�܂�
		void Start() override;

	};

	class TitleScoreUIScript : public Script
	{
	private:
		// ----- �����o -----

		int m_nScore;

		// ����
		std::vector<std::weak_ptr<TitleScoreUIData>> m_data;

	public:
		// �X�R�A�̃Z�b�g
		void SetScore(int nScore) { m_nScore = nScore; }

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

		// UV�X�V
		void UpdateTexCoord();
	};
}
