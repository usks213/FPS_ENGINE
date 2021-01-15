//==================================================================
//								TitleButtonScript.h
//	�^�C�g���{�^�����C�A�E�g
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/15	�^�C�g���{�^���N���X�쐬
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
	class TitleButtonScript : public Script
	{
	private:
		// ----- �����o -----
		std::weak_ptr<SpriteRenderer> m_sprite;
		int m_nCount;

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
