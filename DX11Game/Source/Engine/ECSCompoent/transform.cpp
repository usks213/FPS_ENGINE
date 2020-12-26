//==================================================================
//								Transform.cpp
//	トランスフォーム
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/07/22	トランスフォームクラスの作成
//	2020/09/23	コンポーネント化
//	2020/12/23	ECS用に再設計
//
//===================================================================


//====== インクルード部 ======
#include "Transform.h"
#include "../ECSSystem/TransformSystem.h"

#include "../ECS/Entity/EntityManager.h"

using namespace ECS;

//===== マクロ定義 =====



//========================================
//
//	コンストラクタ
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
//	デストラクタ
//
//========================================
Transform::~Transform()
{

}

//========================================
//
//	生成時
//
//========================================
void Transform::OnCreate()
{
	// システムに格納
	TransformSystem* sys = GetEntityManager()->GetWorld()->GetSystem<TransformSystem>();
	if (sys) sys->AddList(this);
}

//========================================
//
//	破棄時
//
//========================================
void Transform::OnDestroy()
{
	// システムから除外
	TransformSystem* sys = GetEntityManager()->GetWorld()->GetSystem<TransformSystem>();
	if (sys) sys->RemoveList(this);
}

//========================================
//
//	メッセージ受信
//
//========================================
void Transform::SendComponentMessage(const std::string& message, void* value)
{
	// Updateメッセージでマトリックスを更新
	if ("UpdateMatrix" == message)
	{
		UpdateMatrix();
	}
}

//========================================
//
//	更新
//
//========================================
void Transform::UpdateMatrix()
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