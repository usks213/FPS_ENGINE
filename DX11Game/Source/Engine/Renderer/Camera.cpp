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


	CalcWorldMatrix();
	SetCursorPos(SCREEN_CENTER_X, SCREEN_CENTER_Y);

	// 更新
	this->Update();
}

// 更新
void CCamera::Update()
{
	//{
	//	static float angle1;
	//	angle1 = 0;

	//	// カメラ回転
	//	if (GetKeyPress(VK_Z))
	//	{
	//		angle1 += 1.0f;
	//	}
	//	if (GetKeyPress(VK_C))
	//	{
	//		angle1 -= 1.0f;
	//	}

	//	XMMATRIX mR = XMMatrixRotationY(XMConvertToRadians(angle1));
	//	XMVECTOR v = XMVectorSet(m_vPos.x - m_vTarget.x, m_vPos.y - m_vTarget.y, m_vPos.z - m_vTarget.z, 1.0f);

	//	v = XMVector3TransformCoord(v, mR);
	//	XMStoreFloat3(&m_vPos, v);

	//	m_vPos.x += m_vTarget.x;
	//	m_vPos.y += m_vTarget.y;
	//	m_vPos.z += m_vTarget.z;

	//	// 注視点回転
	//	static float angle;
	//	angle = 0.0f;
	//	if (GetKeyPress(VK_Q))
	//	{
	//		angle += 1.0f;
	//	}
	//	if (GetKeyPress(VK_E))
	//	{
	//		angle -= 1.0f;
	//	}

	//	mR = XMMatrixRotationY(XMConvertToRadians(angle));
	//	v = XMVectorSet(m_vTarget.x - m_vPos.x, m_vTarget.y - m_vPos.y, m_vTarget.z - m_vPos.z, 1.0f);

	//	v = XMVector3TransformCoord(v, mR);
	//	XMStoreFloat3(&m_vTarget, v);

	//	m_vTarget.x += m_vPos.x;
	//	m_vTarget.y += m_vPos.y;
	//	m_vTarget.z += m_vPos.z;

	//	m_vAngle.y += angle + angle1;
	//}


	//{	// カメラ移動
	//	float dx = 0.0f, dy = 0.0f, dz = 0.0f;	// 移動量

	//	if (GetKeyPress(VK_W))
	//	{
	//		dx = CAMERA_MOVE_SPEED * CosDeg(-m_vAngle.y + 90);
	//		dz = CAMERA_MOVE_SPEED * SinDeg(-m_vAngle.y + 90);
	//	}
	//	if (GetKeyPress(VK_S))
	//	{
	//		dx = CAMERA_MOVE_SPEED * CosDeg(-m_vAngle.y + 270);
	//		dz = CAMERA_MOVE_SPEED * SinDeg(-m_vAngle.y + 270);
	//	}
	//	if (GetKeyPress(VK_A))
	//	{
	//		dx = CAMERA_MOVE_SPEED * CosDeg(-m_vAngle.y + 180);
	//		dz = CAMERA_MOVE_SPEED * SinDeg(-m_vAngle.y + 180);
	//	}
	//	if (GetKeyPress(VK_D))
	//	{
	//		dx = CAMERA_MOVE_SPEED * CosDeg(-m_vAngle.y);
	//		dz = CAMERA_MOVE_SPEED * SinDeg(-m_vAngle.y);
	//	}
	//	if (GetKeyPress(VK_Y))
	//	{
	//		dy = CAMERA_MOVE_SPEED;
	//	}
	//	if (GetKeyPress(VK_N))
	//	{
	//		dy = -CAMERA_MOVE_SPEED;
	//	}
	//	m_vPos.x += dx;
	//	m_vPos.z += dz;
	//	m_vPos.y += dy;
	//	m_vTarget.x += dx;
	//	m_vTarget.z += dz;

	//	// 注視点移動
	//	dy = 0.0f;
	//	if (GetKeyPress(VK_T))
	//	{
	//		dy = CAMERA_MOVE_SPEED;
	//	}
	//	if (GetKeyPress(VK_B))
	//	{
	//		dy = -CAMERA_MOVE_SPEED;
	//	}

	//	m_vTarget.y += dy;
	//}

// ターゲット座標取得
	//if (m_pTargetPos == nullptr) return;
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

	static bool bFlag = false;
	if (GetKeyTrigger(VK_RETURN)) bFlag ^= 1;
	if (bFlag) return;

	// マウス
	m_vOldPos = Vector3{ (float)SCREEN_CENTER_X, (float)SCREEN_CENTER_Y, 0.0f };
	m_vCurPos = Vector3{ (float)GetMousePosition()->x, (float)GetMousePosition()->y, 0.0f };

	// 移動量
	Vector3 axis = m_vCurPos - m_vOldPos;

	// 戻す
	//m_vCurPos = Vector3{ (float)SCREEN_CENTER_X, (float)SCREEN_CENTER_Y, 0.0f };
	SetCursorPos(SCREEN_CENTER_X, SCREEN_CENTER_Y);
	//SetShowCursor(false);

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
	//if (m_vRelTarget.y >  0.99f) m_vRelTarget.y =  0.99f;
	//if (m_vRelTarget.y < -0.99f) m_vRelTarget.y = -0.99f;

	//m_vRelTarget.x += m_vRelPos.x;
	//m_vRelTarget.y += m_vRelPos.y;
	//m_vRelTarget.z += m_vRelPos.z;


	//===== 垂直 =====
	//mtx = XMMatrixRotationAxis(
	//	XMVectorSet(m_mtxWorld._11,
	//		m_mtxWorld._12,
	//		m_mtxWorld._13, 0.0f),
	//	XMConvertToRadians(axis->y * 0.1f));
	//XMStoreFloat3(&m_vRelPos,
	//	XMVector3TransformCoord(
	//		XMLoadFloat3(&m_vRelPos), mtx));
	//XMStoreFloat3(&m_vRelTarget,
	//	XMVector3TransformCoord(
	//		XMLoadFloat3(&m_vRelTarget), mtx));
	//m_vAngle.x += axis->y * 0.1f;


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

	//// カメラ移動
	//m_vPos.x = m_vPos.x * 0.9f + (pos.x + m_vRelPos.x) * 0.1f;
	//m_vPos.y = m_vPos.y * 0.9f + (pos.y + m_vRelPos.y) * 0.1f;
	//m_vPos.z = m_vPos.z * 0.9f + (pos.z + m_vRelPos.z) * 0.1f;
	//// 注視点移動
	//m_vTarget.x = m_vTarget.x * 0.9f + (pos.x + m_vRelTarget.x) * 0.1f;
	//m_vTarget.y = m_vTarget.y * 0.9f + (pos.y + m_vRelTarget.y) * 0.1f;
	//m_vTarget.z = m_vTarget.z * 0.9f + (pos.z + m_vRelTarget.z) * 0.1f;

	// カメラ移動
	m_vPos.x = pos.x;
	m_vPos.y = pos.y;
	m_vPos.z = pos.z;
	// 注視点移動
	m_vTarget.x = (m_vPos.x + m_vRelTarget.x);
	m_vTarget.y = (m_vPos.y + m_vRelTarget.y);
	m_vTarget.z = (m_vPos.z + m_vRelTarget.z);

	// マトリックス更新
	UpdateMatrix();

	// カメラの向き
	m_vForward->x = m_vRelTarget.x;
	m_vForward->y = m_vRelTarget.y;
	m_vForward->z = m_vRelTarget.z;
	m_vForward = m_vForward.normalized();

	CalcWorldMatrix();// カメラのワールドマトリックス取得

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
