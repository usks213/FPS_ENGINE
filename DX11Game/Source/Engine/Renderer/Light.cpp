#include "Light.h"
#include "../System/input.h"
#include "Camera.h"

using namespace DirectX;

// マクロ定義
#define LIGHT0_DIRECTION	XMFLOAT3(0.0f, -1.0f, 1.0f)
#define LIGHT0_DIFFUSE		XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f)
#define LIGHT0_AMBIENT		XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f)
#define LIGHT0_SPECULAR		XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f)

#define	LIT_POS_P_X			(0.0f)					// カメラの視点初期位置(X座標)
#define	LIT_POS_P_Y			(500.0f)				// カメラの視点初期位置(Y座標)
#define	LIT_POS_P_Z			(0.0f)				// カメラの視点初期位置(Z座標)
#define	LIT_POS_R_X			(0.0f)					// カメラの注視点初期位置(X座標)
#define	LIT_POS_R_Y			(0.0f)					// カメラの注視点初期位置(Y座標)
#define	LIT_POS_R_Z			(0.0f)					// カメラの注視点初期位置(Z座標)


CLight* CLight::m_mainLight = nullptr;


// コンストラクタ
CLight::CLight()
{
	Init();
}

// 初期化
void CLight::Init()
{
	XMFLOAT3 vDir = LIGHT0_DIRECTION;
	XMStoreFloat3(&m_direction, XMVector3Normalize(XMLoadFloat3(&vDir)));
	m_diffuse = LIGHT0_DIFFUSE;
	m_ambient = LIGHT0_AMBIENT;
	m_specular = LIGHT0_SPECULAR;

	// マトリックス作成
	m_vPos = XMFLOAT3(LIT_POS_P_X, LIT_POS_P_Y, LIT_POS_P_Z);		// 視点
	m_vTarget = XMFLOAT3(LIT_POS_R_X, LIT_POS_R_Y, LIT_POS_R_Z);	// 注視点
	m_vUp = XMFLOAT3(0.0f, 1.0f, 0.0f);								// 上方ベクトル

	XMMATRIX m = XMMatrixLookAtLH(XMLoadFloat3(&m_vPos), XMLoadFloat3(&m_vTarget), XMLoadFloat3(&m_vUp));

	//XMMATRIX m2 = XMMATRIX(
	//	0.5f, 0.0f, 0.0f, 0.0f,
	//	0.0f, -0.5f, 0.0f, 0.0f,
	//	0.0f, 0.0f, 1.0f, 0.0f,
	//	0.5f, 0.5f, 0.0f, 1.0f);
	//m = XMMatrixMultiply(m, m2);

	XMStoreFloat4x4(&m_mtxView, m);


	// プロジェクションマトリックス
	XMStoreFloat4x4(&m_mtxProj, XMMatrixOrthographicOffCenterLH(-SCREEN_CENTER_X, SCREEN_CENTER_X, -SCREEN_CENTER_Y, SCREEN_CENTER_Y, 
		VIEW_NEAR_Z, VIEW_FAR_Z));


	vDir = m_vTarget;
	vDir.x -= m_vPos.x;
	vDir.y -= m_vPos.y;
	vDir.z -= m_vPos.z;
	XMStoreFloat3(&m_direction, XMVector3Normalize(XMLoadFloat3(&vDir)));

}


void CLight::Update()
{
	// ライトの向きを変更

	//if (GetKeyPress(VK_L))
	//{
	//	m_vPos.x += 10.f;
	//}
	//if (GetKeyPress(VK_J))
	//{
	//	m_vPos.x -= 10.f;
	//}
	//if (GetKeyPress(VK_I))
	//{
	//	m_vPos.z += 10.f;
	//}
	//if (GetKeyPress(VK_K))
	//{
	//	m_vPos.z -= 10.f;
	//}
	const auto& trans = m_targetTrans.lock();
	if (trans)
	{
		Vector3 pos = trans->m_pos;

		m_vTarget.x = pos->x;
		m_vTarget.y = pos->y;
		m_vTarget.z = pos->z;

		m_vPos.x = m_vTarget.x + LIT_POS_P_X;
		m_vPos.y = m_vTarget.y + LIT_POS_P_Y;
		m_vPos.z = m_vTarget.z + LIT_POS_P_Z;
	}

	// ライトマトリックスの更新
	XMMATRIX m = XMMatrixLookAtLH(XMLoadFloat3(&m_vPos), XMLoadFloat3(&m_vTarget), XMLoadFloat3(&m_vUp));
	XMStoreFloat4x4(&m_mtxView, m);

	// ライトベクトルの更新
	XMFLOAT3 vDir = m_vTarget;
	vDir.x -= m_vPos.x;
	vDir.y -= m_vPos.y;
	vDir.z -= m_vPos.z;
	XMStoreFloat3(&m_direction, XMVector3Normalize(XMLoadFloat3(&vDir)));
}