//==================================================================
//												IObject.h
//	�S�Ă̌��ƂȂ�I�u�W�F�N�g
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/21	�I�u�W�F�N�g�N���X�쐬
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include <memory>


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class IObject
	{
		friend class ObjectManager;

	public:
		// �R���X�g���N�^
		IObject();
		// �f�X�g���N�^
		virtual ~IObject();

		// ���g�̔j��
		virtual void Destroy();

	protected:
		// �I�u�W�F�N�g������
		virtual void OnCreate(){}
		// �I�u�W�F�N�g�j����
		virtual void OnDestroy(){}

	};

}
