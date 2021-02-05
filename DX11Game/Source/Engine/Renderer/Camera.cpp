//=============================================================================
//
// �J���� �N���X [Camera.cpp]
// Author : KOSHI USAMI
//
//=============================================================================
#include "Camera.h"
#include "../System/input.h"
#include "../System/debugproc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	CAM_POS_P_X			(0.0f)					// �J�����̎��_�����ʒu(X���W)
#define	CAM_POS_P_Y			(0.0f)				// �J�����̎��_�����ʒu(Y���W)
#define	CAM_POS_P_Z			(0.0f)				// �J�����̎��_�����ʒu(Z���W)
#define	CAM_POS_R_X			(0.0f)					// �J�����̒����_�����ʒu(X���W)
#define	CAM_POS_R_Y			(0.0f)					// �J�����̒����_�����ʒu(Y���W)
#define	CAM_POS_R_Z			(0.0f)					// �J�����̒����_�����ʒu(Z���W)
#define	VIEW_ANGLE			(80.0f)					// �r���[���ʂ̎���p
#define	VIEW_ASPECT			((float)SCREEN_WIDTH/SCREEN_HEIGHT)	// �r���[���ʂ̃A�X�y�N�g��

#define	VALUE_MOVE_CAMERA	(0.65f)					// �J�����̈ړ���
#define	VALUE_ROTATE_CAMERA	(0.36f)					// �J�����̉�]��
#define	RATE_CHASE_CAMERA_P	(0.09f)					// �J�����̎��_�ւ̕␳�W��

#define CAMERA_MOVE_SPEED	2.0f

#define CAM_DIST_MIN		(20.0f)					// �J�����ŏ�����
#define CAM_DIST_MAX		(3000.0f)				// �J�����ő勗��

#define CAM_ROT_SPEED		(3.0f)

CCamera* CCamera::m_mainCamera = nullptr;


// �R���X�g���N�^
CCamera::CCamera()
{
	Init();
}

// ������
void CCamera::Init()
{
	m_vPos = XMFLOAT3(CAM_POS_P_X, CAM_POS_P_Y, CAM_POS_P_Z);		// ���_
	m_vTarget = XMFLOAT3(CAM_POS_R_X, CAM_POS_R_Y, CAM_POS_R_Z);	// �����_
	m_vUp = XMFLOAT3(0.0f, 1.0f, 0.0f);								// ����x�N�g��
	m_vDestPos = XMFLOAT3(CAM_POS_P_X, CAM_POS_P_Y, CAM_POS_P_Z);
	m_vDestTarget = XMFLOAT3(CAM_POS_R_X, CAM_POS_R_Y, CAM_POS_R_Z);
	m_vVelocity = XMFLOAT3(0.0f, 0.0f, 0.0f);

	m_vAngle = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_vDestAngle = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_vDestTargetAngle = XMFLOAT3(0.0f, 0.0f, 0.0f);

	m_fAspectRatio = VIEW_ASPECT;			// �c����
	m_fFovY = VIEW_ANGLE;					// ����p(Degree)
	m_fNearZ = VIEW_NEAR_Z;					// �O���N���b�v����
	m_fFarZ = VIEW_FAR_Z;					// ����N���b�v����

	float dx, dz;
	dx = m_vPos.x - m_vTarget.x;
	dz = m_vPos.z - m_vTarget.z;
	m_fLengthInterval = sqrtf(dx * dx + dz * dz);

	//m_pTargetPos = nullptr;

	XMFLOAT3 vDir;
	XMStoreFloat3(&vDir, XMVector3Normalize(
		XMVectorSet(m_vPos.x - m_vTarget.x,
			m_vPos.y - m_vTarget.y, m_vPos.z - m_vTarget.z,
			0.0f)));
	m_vRelPos.x = m_vTarget.x + vDir.x * m_fLengthInterval;
	m_vRelPos.y = m_vTarget.y + vDir.y * m_fLengthInterval;
	m_vRelPos.z = m_vTarget.z + vDir.z * m_fLengthInterval;

	m_vRelTarget = { 0, 0, 100 };
	// �����_�ړ�
	m_vTarget.x = (m_vPos.x + m_vRelTarget.x);
	m_vTarget.y = (m_vPos.y + m_vRelTarget.y);
	m_vTarget.z = (m_vPos.z + m_vRelTarget.z);


	m_bCursorFlg = false;

	CalcWorldMatrix();
	SetCursorPos(SCREEN_CENTER_X, SCREEN_CENTER_Y);

	// �X�V
	this->Update();
}

// �X�V
void CCamera::Update()
{
	// �^�[�Q�b�g���W�擾
	const auto& trans = m_targetTrans.lock();
	if (!trans) return;

	XMFLOAT3 pos = *trans->m_pos.GetFloat3();

	// �J�����ړ�
	m_vPos.x = pos.x;
	m_vPos.y = pos.y;
	m_vPos.z = pos.z;

	// �J�����ړ�
	if (GetKeyPress(VK_Q)) {
		m_fLengthInterval -= 5.0f;
		if (m_fLengthInterval < CAM_DIST_MIN)
			m_fLengthInterval = CAM_DIST_MIN;
		XMFLOAT3 vDir;
		XMStoreFloat3(&vDir, XMVector3Normalize(
			XMVectorSet(m_vPos.x - m_vTarget.x,
				m_vPos.y - m_vTarget.y, m_vPos.z - m_vTarget.z,
				0.0f)));
		m_vRelPos.x = m_vTarget.x + vDir.x * m_fLengthInterval - pos.x;
		m_vRelPos.y = m_vTarget.y + vDir.y * m_fLengthInterval - pos.y;
		m_vRelPos.z = m_vTarget.z + vDir.z * m_fLengthInterval - pos.z;
	}
	if (GetKeyPress(VK_E)) {
		m_fLengthInterval += 5.0f;
		if (m_fLengthInterval > CAM_DIST_MAX)
			m_fLengthInterval = CAM_DIST_MAX;
		XMFLOAT3 vDir;
		XMStoreFloat3(&vDir, XMVector3Normalize(
			XMVectorSet(m_vPos.x - m_vTarget.x,
				m_vPos.y - m_vTarget.y, m_vPos.z - m_vTarget.z,
				0.0f)));
		m_vRelPos.x = m_vTarget.x + vDir.x * m_fLengthInterval - pos.x;
		m_vRelPos.y = m_vTarget.y + vDir.y * m_fLengthInterval - pos.y;
		m_vRelPos.z = m_vTarget.z + vDir.z * m_fLengthInterval - pos.z;
	}

	if (GetKeyTrigger(VK_RETURN)) m_bCursorFlg ^= 1;
	if (!m_bCursorFlg)
	{
		// �}�E�X
		m_vOldPos = Vector3{ (float)SCREEN_CENTER_X, (float)SCREEN_CENTER_Y, 0.0f };
		m_vCurPos = Vector3{ (float)GetMousePosition()->x, (float)GetMousePosition()->y, 0.0f };

		// �ړ���
		Vector3 axis = m_vCurPos - m_vOldPos;

		// �߂�
		//m_vCurPos = Vector3{ (float)SCREEN_CENTER_X, (float)SCREEN_CENTER_Y, 0.0f };
		SetCursorPos(SCREEN_CENTER_X, SCREEN_CENTER_Y);
		SetShowCursor(false);

		//===== ���� =====
		XMMATRIX mtx = XMMatrixRotationY(
			XMConvertToRadians(axis->x * 0.1f));

		// �X�V
		XMVECTOR v = XMVectorSet(
			m_vRelTarget.x, m_vRelTarget.y, m_vRelTarget.z, 1.0f);
		v = XMVector3TransformCoord(v, mtx);
		v = XMVector3Normalize(v);
		XMStoreFloat3(&m_vRelTarget, v);


		//===== ���� =====
		mtx = XMMatrixRotationAxis(
			XMVectorSet(m_mtxWorld._11,
				m_mtxWorld._12,
				m_mtxWorld._13, 0.0f),
			XMConvertToRadians(axis->y * 0.1f));

		// �o�b�N�A�b�v
		XMFLOAT3 back = m_vRelTarget;

		// �X�V
		v = XMVectorSet(
			m_vRelTarget.x, m_vRelTarget.y, m_vRelTarget.z, 1.0f);
		v = XMVector3TransformCoord(v, mtx);
		v = XMVector3Normalize(v);
		XMStoreFloat3(&m_vRelTarget, v);

		// �␳ 
		if (m_vRelTarget.y > 0.99f || m_vRelTarget.y < -0.99f)
		{
			m_vRelTarget = back;
		}
	}
	else
	{
		SetShowCursor(true);
	}

	// �J��������
	if (GetKeyPress(VK_RIGHT)) {
		XMMATRIX mtx = XMMatrixRotationY(
			XMConvertToRadians(CAM_ROT_SPEED));
		XMStoreFloat3(&m_vRelPos,
			XMVector3TransformCoord(
				XMLoadFloat3(&m_vRelPos), mtx));
		XMStoreFloat3(&m_vRelTarget,
			XMVector3TransformCoord(
				XMLoadFloat3(&m_vRelTarget), mtx));
		m_vAngle.y += CAM_ROT_SPEED;
		if (m_vAngle.y >= 180.0f)
			m_vAngle.y -= 360.0f;
	}
	if (GetKeyPress(VK_LEFT)) {
		XMMATRIX mtx = XMMatrixRotationY(
			XMConvertToRadians(-CAM_ROT_SPEED));
		XMStoreFloat3(&m_vRelPos,
			XMVector3TransformCoord(
				XMLoadFloat3(&m_vRelPos), mtx));
		XMStoreFloat3(&m_vRelTarget,
			XMVector3TransformCoord(
				XMLoadFloat3(&m_vRelTarget), mtx));
		m_vAngle.y -= CAM_ROT_SPEED;
		if (m_vAngle.y < -180.0f)
			m_vAngle.y += 360.0f;
	}
	if (GetKeyPress(VK_DOWN) && m_vAngle.x + 1 < 80.0f) {
		XMMATRIX mtx = XMMatrixRotationAxis(
			XMVectorSet(m_mtxWorld._11,
				m_mtxWorld._12,
				m_mtxWorld._13, 0.0f),
			XMConvertToRadians(CAM_ROT_SPEED));
		XMStoreFloat3(&m_vRelPos,
			XMVector3TransformCoord(
				XMLoadFloat3(&m_vRelPos), mtx));
		XMStoreFloat3(&m_vRelTarget,
			XMVector3TransformCoord(
				XMLoadFloat3(&m_vRelTarget), mtx));
		m_vAngle.x += CAM_ROT_SPEED;
	}
	if (GetKeyPress(VK_UP) && m_vAngle.x - 1 > -80.0f) {
		XMMATRIX mtx = XMMatrixRotationAxis(
			XMVectorSet(m_mtxWorld._11,
				m_mtxWorld._12,
				m_mtxWorld._13, 0.0f),
			XMConvertToRadians(-CAM_ROT_SPEED));
		XMStoreFloat3(&m_vRelPos,
			XMVector3TransformCoord(
				XMLoadFloat3(&m_vRelPos), mtx));
		XMStoreFloat3(&m_vRelTarget,
			XMVector3TransformCoord(
				XMLoadFloat3(&m_vRelTarget), mtx));
		m_vAngle.x -= CAM_ROT_SPEED;
	}

	// �J�����̌���
	m_vForward->x = m_vRelTarget.x;
	m_vForward->y = m_vRelTarget.y;
	m_vForward->z = m_vRelTarget.z;
	m_vForward = m_vForward.normalized();

	// ��ʗh��
	static float fOs = 50;
	static float fSpeed = 2000 / 60.0f;
	static float yOff = 0;
	static bool  bUp = true;
	if (m_nShakeFrame > 0)
	{
		//// �E�x�N�g��
		//Vector3 up = Vector3{ 0.0f, 1.0f, 0.0f };
		//m_vRight = Vector3::Cross(m_vForward, up).normalized();
		//up = Vector3::Cross(m_vForward, m_vRight).normalized();

		//// �h�炷�I�t�Z�b�g
		//const float fOs = 100;
		//const float fspeed = 50 / 60;

		//int ra1 = rand() % 100 - 50;
		//int ra2 = rand() % 100 - 50;
		//m_vShakeOffset = m_vRight * ra1;
		//m_vShakeOffset = up * ra2;

		yOff += fSpeed;
		if (yOff > fOs && bUp)
		{
			fOs *= 0.8f;
			fSpeed *= -1;
			bUp = false;
		}
		if (yOff < -fOs && !bUp)
		{
			fOs *= 0.8f;
			fSpeed *= -1;
			bUp = true;
		}
		m_vShakeOffset->y = -yOff;

		m_nShakeFrame--;
	}
	else
	{
		m_vShakeOffset = Vector3{ 0,0,0 };
		fOs = 50;
		fSpeed = 2000 / 60.0f;
		yOff = 0;
		bUp = true;
	}

	// �J�����ړ�
	m_vPos.x = pos.x + m_vShakeOffset->x;
	m_vPos.y = pos.y + m_vShakeOffset->y;
	m_vPos.z = pos.z + m_vShakeOffset->z;

	// �����_�ړ�
	m_vTarget.x = (m_vPos.x + m_vRelTarget.x);
	m_vTarget.y = (m_vPos.y + m_vRelTarget.y);
	m_vTarget.z = (m_vPos.z + m_vRelTarget.z);

	// �}�g���b�N�X�X�V
	UpdateMatrix();

	// �J�����̃��[���h�}�g���b�N�X�擾
	CalcWorldMatrix();
}

// �r���[/�v���W�F�N�V���� �}�g���b�N�X�X�V
void CCamera::UpdateMatrix()
{
	XMStoreFloat4x4(&m_mtxView, XMMatrixLookAtLH(
		XMLoadFloat3(&m_vPos), XMLoadFloat3(&m_vTarget), XMLoadFloat3(&m_vUp)));
	XMStoreFloat4x4(&m_mtxProj, XMMatrixPerspectiveFovLH(
		XMConvertToRadians(m_fFovY), m_fAspectRatio, m_fNearZ, m_fFarZ));
}

// ���[���h �}�g���b�N�X�ݒ�
void CCamera::SetWorldMatrix(XMFLOAT4X4& mtxWorld)
{
	m_mtxWorld = mtxWorld;
	m_vPos = XMFLOAT3(mtxWorld._41, mtxWorld._42, mtxWorld._43);
	m_vTarget = XMFLOAT3(mtxWorld._41 + mtxWorld._31, mtxWorld._42 + mtxWorld._32, mtxWorld._43 + mtxWorld._33);
	m_vUp = XMFLOAT3(mtxWorld._21, mtxWorld._22, mtxWorld._23);
}

// ���_/�����_/����x�N�g�����烏�[���h �}�g���b�N�X�Z�o
XMFLOAT4X4& CCamera::CalcWorldMatrix()
{
	XMVECTOR vecZ = XMVectorSet(m_vTarget.x - m_vPos.x, m_vTarget.y - m_vPos.y, m_vTarget.z - m_vPos.z, 0.0f);
	XMFLOAT3 vZ;
	XMStoreFloat3(&vZ, XMVector3Normalize(vecZ));
	XMVECTOR vecY = XMLoadFloat3(&m_vUp);
	XMVECTOR vecX = XMVector3Normalize(XMVector3Cross(vecY, vecZ));
	XMFLOAT3 vX;
	XMStoreFloat3(&vX, vecX);
	vecY = XMVector3Normalize(XMVector3Cross(vecZ, vecX));
	XMFLOAT3 vY;
	XMStoreFloat3(&vY, vecY);

	m_mtxWorld._11 = vX.x;
	m_mtxWorld._12 = vX.y;
	m_mtxWorld._13 = vX.z;
	m_mtxWorld._14 = 0.0f;
	m_mtxWorld._21 = vY.x;
	m_mtxWorld._22 = vY.y;
	m_mtxWorld._23 = vY.z;
	m_mtxWorld._24 = 0.0f;
	m_mtxWorld._31 = vZ.x;
	m_mtxWorld._32 = vZ.y;
	m_mtxWorld._33 = vZ.z;
	m_mtxWorld._34 = 0.0f;
	m_mtxWorld._41 = m_vPos.x;
	m_mtxWorld._42 = m_vPos.y;
	m_mtxWorld._43 = m_vPos.z;
	m_mtxWorld._44 = 1.0f;

	return m_mtxWorld;
}
