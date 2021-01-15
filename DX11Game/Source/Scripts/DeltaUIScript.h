//==================================================================
//								DeltaUIScript.h
//	�����\��UI
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/15	���\��UI�N���X�쐬
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include "../Engine/ECSCompoent/Script.h"
#include "PlayerScript.h"
#include "../Engine/ECSCompoent/SpriteRenderer.h"


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class DeltaUIScript;

	class DeltaUIData :public Script
	{
		friend DeltaUIScript;
	private:
		// ----- �����o -----
		std::weak_ptr<SpriteRenderer> m_sprite;
		int m_nAnimNo;

	public:
		// ----- ���\�b�h -----

		// �J�n���ɌĂ΂�܂�
		void Start() override;

	};

	class DeltaUIScript : public Script
	{
	private:
		// ----- �����o -----

		// �v���C���[
		std::weak_ptr<PlayerScript> m_player;

		// ����
		std::vector<std::weak_ptr<DeltaUIData>> m_data;

	public:
		// �v���C���[�̎擾
		void SetPlayer(std::weak_ptr<PlayerScript> player) { m_player = player; }

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
