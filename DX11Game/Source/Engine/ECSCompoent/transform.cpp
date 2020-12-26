//==================================================================
//								Transform.cpp
//	�g�����X�t�H�[��
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/07/22	�g�����X�t�H�[���N���X�̍쐬
//	2020/09/23	�R���|�[�l���g��
//	2020/12/23	ECS�p�ɍĐ݌v
//
//===================================================================


//====== �C���N���[�h�� ======
#include "Transform.h"
#include "../ECSSystem/TransformSystem.h"

#include "../ECS/Entity/EntityManager.h"

using namespace ECS;

//===== �}�N����` =====



//========================================
//
//	�R���X�g���N�^
//
//========================================
Transform::Transform()
{
	m_pos = Vector3{ 0.0f, 0.0f, 0.0f };
	m_scale = Vector3{ 1.0f, 1.0f, 1.0f };
	m_rot = Vector3{ 0.0f, 0.0f, 0.0f };

}

//========================================
//
//	�f�X�g���N�^
//
//========================================
Transform::~Transform()
{

}

//========================================
//
//	������
//
//========================================
void Transform::OnCreate()
{
	// �V�X�e���Ɋi�[
	TransformSystem* sys = GetEntityManager()->GetWorld()->GetSystem<TransformSystem>();
	if (sys) sys->AddList(this);
}

//========================================
//
//	�j����
//
//========================================
void Transform::OnDestroy()
{
	// �V�X�e�����珜�O
	TransformSystem* sys = GetEntityManager()->GetWorld()->GetSystem<TransformSystem>();
	if (sys) sys->RemoveList(this);
}

//========================================
//
//	���b�Z�[�W��M
//
//========================================
void Transform::SendComponentMessage(const std::string& message, void* value)
{
	// Update���b�Z�[�W�Ń}�g���b�N�X���X�V
	if ("UpdateMatrix" == message)
	{
		UpdateMatrix();
	}
}

//========================================
//
//	�X�V
//
//========================================
void Transform::UpdateMatrix()
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