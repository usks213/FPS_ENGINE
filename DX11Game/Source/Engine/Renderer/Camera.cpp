//=============================================================================
//
// カメラ クラス [Camera.cpp]
// Author : KOSHI USAMI
//
//=============================================================================
#include "Camera.h"
#include "../System/input.h"
#include "../System/debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	CAM_POS_P_X			(0.0f)					// カメラの視点初期位置(X座標)
#define	CAM_POS_P_Y			(0.0f)				// カメラの視点初期位置(Y座標)
#define	CAM_POS_P_Z			(0.0f)				// カメラの視点初期位置(Z座標)
#define	CAM_POS_R_X			(0.0f)					// カメラの注視点初期位置(X座標)
#define	CAM_POS_R_Y			(0.0f)					// カメラの注視点初期位置(Y座標)
#define	CAM_POS_R_Z			(0.0f)					// カメラの注視点初期位置(Z座標)
#define	VIEW_ANGLE			(80.0f)					// ビュー平面の視野角
#define	VIEW_ASPECT			((float)SCREEN_WIDTH/SCREEN_HEIGHT)	// ビュー平面のアスペクト比

#define	VALUE_MOVE_CAMERA	(0.65f)					// カメラの移動量
#define	VALUE_ROTATE_CAMERA	(0.36f)					// カメラの回転量
#define	RATE_CHASE_CAMERA_P	(0.09f)					// カメラの視点への補正係数

#define CAMERA_MOVE_SPEED	2.0f

#define CAM_DIST_MIN		(20.0f)					// カメラ最小距離
#define CAM_DIST_MAX		(3000.0f)				// カメラ最大距離

CCamera* CCamera::m_mainCamera = nullptr;


// コンストラクタ
CCamera::CCamera()
{
	Init();
}

// 初期化
void CCamera::Init()
{
	m_vPos = XMFLOAT3(CAM_POS_P_X, CAM_POS_P_Y, CAM_POS_P_Z);		// 視点
	m_vTarget = XMFLOAT3(CAM_POS_R_X, CAM_POS_R_Y, CAM_POS_R_Z);	// 注視点
	m_vUp = XMFLOAT3(0.0f, 1.0f, 0.0f);								// 上方ベクトル
	m_vDestPos = XMFLOAT3(CAM_POS_P_X, CAM_POS_P_Y, CAM_POS_P_Z);
	m_vDestTarget = XMFLOAT3(CAM_POS_R_X, CAM_POS_R_Y, CAM_POS_R_Z);
	m_vVelocity = XMFLOAT3(0.0f, 0.0f, 0.0f);

	m_vAngle = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_vDestAngle = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_vDestTargetAngle = XMFLOAT3(0.0f, 0.0f, 0.0f);

	m_fAspectRatio = VIEW_ASPECT;			// 縦横比
	m_fFovY = VIEW_ANGLE;					// 視野角(Degree)
	m_fNearZ = VIEW_NEAR_Z;					// 前方クリップ距離
	m_fFarZ = VIEW_FAR_Z;					// 後方クリップ距離

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
	// 注視点移動
	m_vTarget.x = (m_vPos.x + m_vRelTarget.x);
	m_vTarget.y = (m_vPos.y + m_vRelTarget.y);
	m_vTarget.z = (m_vPos.z + m_vRelTarget.z);


	m_bCursorFlg = false;

	CalcWorldMatrix();
	SetCursorPos(SCREEN_CENTER_X, SCREEN_CENTER_Y);

	// 更新
	this->Update();
}

// 更新
void CCamera::Update()
{
	// ターゲット座標取得
	const auto& trans = m_targetTrans.lock();
	if (!trans) return;

	XMFLOAT3 pos = *trans->m_pos.GetFloat3();

	// カメラ移動
	m_vPos.x = pos.x;
	m_vPos.y = pos.y;
	m_vPos.z = pos.z;

	// カメラ移動
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
		// マウス
		m_vOldPos = Vector3{ (float)SCREEN_CENTER_X, (float)SCREEN_CENTER_Y, 0.0f };
		m_vCurPos = Vector3{ (float)GetMousePosition()->x, (float)GetMousePosition()->y, 0.0f };

		// 移動量
		Vector3 axis = m_vCurPos - m_vOldPos;

		// 戻す
		//m_vCurPos = Vector3{ (float)SCREEN_CENTER_X, (float)SCREEN_CENTER_Y, 0.0f };
		SetCursorPos(SCREEN_CENTER_X, SCREEN_CENTER_Y);
		SetShowCursor(false);

		//===== 水平 =====
		XMMATRIX mtx = XMMatrixRotationY(
			XMConvertToRadians(axis->x * 0.1f));

		// 更新
		XMVECTOR v = XMVectorSet(
			m_vRelTarget.x, m_vRelTarget.y, m_vRelTarget.z, 1.0f);
		v = XMVector3TransformCoord(v, mtx);
		v = XMVector3Normalize(v);
		XMStoreFloat3(&m_vRelTarget, v);


		//===== 垂直 =====
		mtx = XMMatrixRotationAxis(
			XMVectorSet(m_mtxWorld._11,
				m_mtxWorld._12,
				m_mtxWorld._13, 0.0f),
			XMConvertToRadians(axis->y * 0.1f));

		// バックアップ
		XMFLOAT3 back = m_vRelTarget;

		// 更新
		v = XMVectorSet(
			m_vRelTarget.x, m_vRelTarget.y, m_vRelTarget.z, 1.0f);
		v = XMVector3TransformCoord(v, mtx);
		v = XMVector3Normalize(v);
		XMStoreFloat3(&m_vRelTarget, v);

		// 補正 
		if (m_vRelTarget.y > 0.99f || m_vRelTarget.y < -0.99f)
		{
			m_vRelTarget = back;
		}
	}
	else
	{
		SetShowCursor(true);
	}

	// カメラ操作
	if (GetKeyPress(VK_RIGHT)) {
		XMMATRIX mtx = XMMatrixRotationY(
			XMConvertToRadians(1));
		XMStoreFloat3(&m_vRelPos,
			XMVector3TransformCoord(
				XMLoadFloat3(&m_vRelPos), mtx));
		XMStoreFloat3(&m_vRelTarget,
			XMVector3TransformCoord(
				XMLoadFloat3(&m_vRelTarget), mtx));
		m_vAngle.y += 1.0f;
		if (m_vAngle.y >= 180.0f)
			m_vAngle.y -= 360.0f;
	}
	if (GetKeyPress(VK_LEFT)) {
		XMMATRIX mtx = XMMatrixRotationY(
			XMConvertToRadians(-1));
		XMStoreFloat3(&m_vRelPos,
			XMVector3TransformCoord(
				XMLoadFloat3(&m_vRelPos), mtx));
		XMStoreFloat3(&m_vRelTarget,
			XMVector3TransformCoord(
				XMLoadFloat3(&m_vRelTarget), mtx));
		m_vAngle.y -= 1.0f;
		if (m_vAngle.y < -180.0f)
			m_vAngle.y += 360.0f;
	}
	if (GetKeyPress(VK_DOWN) && m_vAngle.x + 1 < 80.0f) {
		XMMATRIX mtx = XMMatrixRotationAxis(
			XMVectorSet(m_mtxWorld._11,
				m_mtxWorld._12,
				m_mtxWorld._13, 0.0f),
			XMConvertToRadians(1));
		XMStoreFloat3(&m_vRelPos,
			XMVector3TransformCoord(
				XMLoadFloat3(&m_vRelPos), mtx));
		XMStoreFloat3(&m_vRelTarget,
			XMVector3TransformCoord(
				XMLoadFloat3(&m_vRelTarget), mtx));
		m_vAngle.x += 1.0f;
	}
	if (GetKeyPress(VK_UP) && m_vAngle.x - 1 > -80.0f) {
		XMMATRIX mtx = XMMatrixRotationAxis(
			XMVectorSet(m_mtxWorld._11,
				m_mtxWorld._12,
				m_mtxWorld._13, 0.0f),
			XMConvertToRadians(-1));
		XMStoreFloat3(&m_vRelPos,
			XMVector3TransformCoord(
				XMLoadFloat3(&m_vRelPos), mtx));
		XMStoreFloat3(&m_vRelTarget,
			XMVector3TransformCoord(
				XMLoadFloat3(&m_vRelTarget), mtx));
		m_vAngle.x -= 1.0f;
	}

	// カメラの向き
	m_vForward->x = m_vRelTarget.x;
	m_vForward->y = m_vRelTarget.y;
	m_vForward->z = m_vRelTarget.z;
	m_vForward = m_vForward.normalized();

	// 画面揺れ
	static float fOs = 50;
	static float fSpeed = 2000 / 60.0f;
	static float yOff = 0;
	static bool  bUp = true;
	if (m_nShakeFrame > 0)
	{
		//// 右ベクトル
		//Vector3 up = Vector3{ 0.0f, 1.0f, 0.0f };
		//m_vRight = Vector3::Cross(m_vForward, up).normalized();
		//up = Vector3::Cross(m_vForward, m_vRight).normalized();

		//// 揺らすオフセット
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

	// カメラ移動
	m_vPos.x = pos.x + m_vShakeOffset->x;
	m_vPos.y = pos.y + m_vShakeOffset->y;
	m_vPos.z = pos.z + m_vShakeOffset->z;

	// 注視点移動
	m_vTarget.x = (m_vPos.x + m_vRelTarget.x);
	m_vTarget.y = (m_vPos.y + m_vRelTarget.y);
	m_vTarget.z = (m_vPos.z + m_vRelTarget.z);

	// マトリックス更新
	UpdateMatrix();

	// カメラのワールドマトリックス取得
	CalcWorldMatrix();
}

// ビュー/プロジェクション マトリックス更新
void CCamera::UpdateMatrix()
{
	XMStoreFloat4x4(&m_mtxView, XMMatrixLookAtLH(
		XMLoadFloat3(&m_vPos), XMLoadFloat3(&m_vTarget), XMLoadFloat3(&m_vUp)));
	XMStoreFloat4x4(&m_mtxProj, XMMatrixPerspectiveFovLH(
		XMConvertToRadians(m_fFovY), m_fAspectRatio, m_fNearZ, m_fFarZ));
}

// ワールド マトリックス設定
void CCamera::SetWorldMatrix(XMFLOAT4X4& mtxWorld)
{
	m_mtxWorld = mtxWorld;
	m_vPos = XMFLOAT3(mtxWorld._41, mtxWorld._42, mtxWorld._43);
	m_vTarget = XMFLOAT3(mtxWorld._41 + mtxWorld._31, mtxWorld._42 + mtxWorld._32, mtxWorld._43 + mtxWorld._33);
	m_vUp = XMFLOAT3(mtxWorld._21, mtxWorld._22, mtxWorld._23);
}

// 視点/注視点/上方ベクトルからワールド マトリックス算出
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
