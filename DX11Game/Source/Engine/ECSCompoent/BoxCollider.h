//==================================================================
//								BoxCollider.h
//	�{�b�N�X�R���C�_�[
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/27	�{�b�N�X�R���C�_�[�N���X�쐬
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include "Collider.h"

//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class BoxCollider final : public Collider
	{
	
	public:
		// �R���X�g���N�^
		BoxCollider();
		// �f�X�g���N�^
		~BoxCollider();

		// �����蔻��T�C�Y
		void SetSize(Vector3 size) { m_bound.SetSize(size); }

	protected:
		// �e�R���C�_�[�̓����蔻��
		bool Judgment(Collider* other) override;
	};
}
