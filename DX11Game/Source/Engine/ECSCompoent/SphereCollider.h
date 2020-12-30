//==================================================================
//								SphereCollider.h
//	�X�t�B�A�R���C�_�[
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/27	�X�t�B�A�R���C�_�[�N���X�쐬
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include "Collider.h"

//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class SphereCollider final : public Collider
	{
	
	public:
		// �R���X�g���N�^
		SphereCollider();
		// �f�X�g���N�^
		~SphereCollider();

		// �����蔻��T�C�Y
		void SetRadius(float radius) { m_bound.SetSize(Vector3(2,2,2) * radius); }

	protected:
		// �e�R���C�_�[�̓����蔻��
		bool Judgment(Collider* other) override;
	};
}
