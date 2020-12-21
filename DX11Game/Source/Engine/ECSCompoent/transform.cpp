//==================================================================
//								transform.cpp
//	トランスフォーム
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/07/22	トランスフォームクラスの作成
//
//===================================================================


//====== インクルード部 ======
#include "transform.h"


//===== マクロ定義 =====



//========================================
//
//	コンストラクタ
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
//	デストラクタ
//
//========================================
CTransform::~CTransform()
{

}


//========================================
//
//	更新
//
//========================================
void CTransform::LateUpdate()
{
	XMMATRIX mtxWorld, mtxScale, mtxRot, mtxTranslate;

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// 拡大縮小
	mtxScale = XMMatrixScaling(m_scale->x, m_scale->y, m_scale->z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScale);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_rot->x),
		XMConvertToRadians(m_rot->y), XMConvertToRadians(m_rot->z));
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(m_pos->x, m_pos->y, m_pos->z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックス設定
	XMStoreFloat4x4(&m_mtxWorld, mtxWorld);
}