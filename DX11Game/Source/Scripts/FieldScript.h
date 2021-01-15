//==================================================================
//								FieldScript.h
//	�t�B�[���h
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/14	�t�B�[���h�N���v�g�N���X�쐬
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include "../Engine/ECSCompoent/Script.h"
#include "../Engine/ECSCompoent/MeshRenderer.h"


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class FieldScript : public Script
	{
	private:
		// ----- �����o -----
		std::weak_ptr<GameObject> m_target;
		std::weak_ptr<MeshRenderer> m_renderer;

	public:
		// �^�[�Q�b�g
		void SetTarget(std::weak_ptr<GameObject> target) { m_target = target; }

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
