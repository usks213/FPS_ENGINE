//==================================================================
//								transform.h
//	�g�����X�t�H�[��
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/07/22	�g�����X�t�H�[���N���X�̍쐬
//	2020/09/23	�R���|�[�l���g��
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include "CComponent.h"

//===== �}�N����` =====


//===== �N���X��` =====
class CTransform : public CComponent
{
public:
	CTransform();
	~CTransform();

	void LateUpdate() override;

	// �Q�b�g�֐�
	Vector3 GetRotation() { return m_rot; }
	Vector3 GetPos() { return m_pos; }
	Vector3 GetScale() { return m_scale; }
	Vector3 GetDir() { return m_dir; }
	XMFLOAT4X4* GetWorldMatrix() { return &m_mtxWorld; }
	// �Z�b�g�֐�
	void SetPos(Vector3 pos) { m_pos = pos; }
	void SetRotation(Vector3 rot) { m_rot = rot; }
	void SetScale(Vector3 scale) { m_scale = scale; }
	void SetDir(Vector3 dir) { m_dir = dir; }

	Vector3 m_pos;				// ���S���W
	Vector3 m_scale;				// �c���T�C�Y
	Vector3 m_rot;				// �I�u�W�F�N�g�̉�]�p�x
	Vector3 m_dir;				// ����
	XMFLOAT4X4 m_mtxWorld;

private:

};

