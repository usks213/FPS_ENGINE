//==================================================================
//								transform.cpp
//	�g�����X�t�H�[��
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/07/22	�g�����X�t�H�[���N���X�̍쐬
//
//===================================================================


//====== �C���N���[�h�� ======
#include "transform.h"


//===== �}�N����` =====



//========================================
//
//	�R���X�g���N�^
//
//========================================
CTransform::CTransform()
{
	m_pos = Vector3{ 0.0f, 0.0f, 0.0f };
	m_scale = Vector3{ 1.0f, 1.0f, 1.0f };
	m_rot = Vector3{ 0.0f, 0.0f, 0.0f };
	m_dir = Vector3{ 0.0f, 0.0f, 0.0f };

	LateUpdate();
}

//========================================
//
//	�f�X�g���N�^
//
//========================================
CTransform::~CTransform()
{

}


//========================================
//
//	�X�V
//
//========================================
void CTransform::LateUpdate()
{
	XMMATRIX mtxWorld, mtxScale, mtxRot, mtxTranslate;

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	// �g��k��
	mtxScale = XMMatrixScaling(m_scale->x, m_scale->y, m_scale->z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScale);

	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_rot->x),
		XMConvertToRadians(m_rot->y), XMConvertToRadians(m_rot->z));
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(m_pos->x, m_pos->y, m_pos->z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�ݒ�
	XMStoreFloat4x4(&m_mtxWorld, mtxWorld);
}