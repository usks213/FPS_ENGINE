//==================================================================
//								Rigidbody.cpp
//	リジッドボディ
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/27	リジッドボディクラス作成
//
//===================================================================


//====== インクルード部 ======
#include "Rigidbody.h"
#include "../ECSSystem/RigidbodySystem.h"

#include "../ECS/Entity/EntityManager.h"
#include "Transform.h"


using namespace ECS;

//===== マクロ定義 =====
#define GRAVITY (9.8f / 60.0f)
#define DRAG (1.0f)

#define MAX_VELOCITY (50.0f)


//========================================
//
//	コンストラクタ
//
//========================================
Rigidbody::Rigidbody()
{
	// 移動
	m_velocity			= Vector3{0.0f, 0.0f, 0.0f};
	m_force				= Vector3{ 0.0f, 0.0f, 0.0f };
	m_drag				= Vector3{ 0.05f, 0.0f, 0.05f };
	// 回転
	m_angularVelocity	= Vector3{ 0.0f, 0.0f, 0.0f };
	m_torque			= Vector3{ 0.0f, 0.0f, 0.0f };
	m_torqueDrag		= Vector3{ 0.05f, 0.05f, 0.05f };
	// 物理使用
	m_bUsePhysics		= true;
	// 重力
	m_bUseGravity		= true;
	m_fGraviyForce		= Vector3{ 0.0f, -GRAVITY, 0.0f };
	// 質量
	m_fMass				= 1.0f;
	// 摩擦
	m_fStaticFriction	= 0.5f;	// 静止
	m_fDynamicFriction	= 0.3f;	// 動
	// 反発係数
	m_e					= 0.0f;
}

//========================================
//
//	デストラクタ
//
//========================================
Rigidbody::~Rigidbody()
{

}

//========================================
//
//	生成時
//
//========================================
void Rigidbody::OnCreate()
{
	// システムに格納
	RigidbodySystem* sys = GetEntityManager()->GetWorld()->GetSystem<RigidbodySystem>();
	if (sys) sys->AddList(this);

	// トランスフォーム取得
	m_transform = m_Parent.lock()->GetComponent<Transform>();
	// ゲームオブジェクト取得
	m_gameObject = m_transform.lock()->gameObject();
}

//========================================
//
//	破棄時
//
//========================================
void Rigidbody::OnDestroy()
{
	// システムから除外
	RigidbodySystem* sys = GetEntityManager()->GetWorld()->GetSystem<RigidbodySystem>();
	if (sys) sys->RemoveList(this);
}

//========================================
//
//	メッセージ受信
//
//========================================
void Rigidbody::SendComponentMessage(const std::string& message, void* value)
{
	// Updateメッセージ
	if ("Update" == message)
	{
	}
}

//========================================
//
//	更新
//
//========================================
void Rigidbody::Update()
{
	// トランスフォーム
	const auto& pTrans = transform().lock();
	// 座標・回転
	Vector3 pos = pTrans->m_pos;
	Vector3 rot = pTrans->m_rot;

	//===== 回転 =====

	// 移動量
	m_angularVelocity->x = 0;
	m_angularVelocity->y = 0;
	m_angularVelocity->z = 0;

	// 外力
	m_angularVelocity->x += m_torque->x;
	m_angularVelocity->y += m_torque->y;
	m_angularVelocity->z += m_torque->z;

	// 回転の更新
	rot->x += m_angularVelocity->x;
	rot->y += m_angularVelocity->y;
	rot->z += m_angularVelocity->z;
	pTrans->m_rot = rot;

	// 抵抗で加速度を減らす	fDrag(0.0f ～ 1.0f)
	m_torque->x *= (1.0f - m_torqueDrag->x);
	if (fabsf(m_torque->x) < 0.01f) m_torque->x = 0.0f;
	m_torque->y *= (1.0f - m_torqueDrag->y);
	if (fabsf(m_torque->y) < 0.01f) m_torque->y = 0.0f;
	m_torque->z *= (1.0f - m_torqueDrag->z);
	if (fabsf(m_torque->z) < 0.01f) m_torque->z = 0.0f;


	//===== 移動 =====

	// 移動量
	m_velocity->x = 0;
	m_velocity->y = 0;
	m_velocity->z = 0;

	// 重力
	if (m_bUseGravity)
	{
		m_force += m_fGraviyForce * m_fMass;
	}
	// 重力をオン
	if (!m_bUsePhysics)
		m_bUseGravity = true;

	// 外力	外力はAddForceに
	m_velocity->x += m_force->x;
	m_velocity->y += m_force->y;
	m_velocity->z += m_force->z;

	// 速度限界
	if (m_velocity->x > MAX_VELOCITY) m_velocity->x = MAX_VELOCITY;
	if (m_velocity->y > MAX_VELOCITY) m_velocity->y = MAX_VELOCITY;
	if (m_velocity->z > MAX_VELOCITY) m_velocity->z = MAX_VELOCITY;
	if (m_velocity->x < -MAX_VELOCITY) m_velocity->x = -MAX_VELOCITY;
	if (m_velocity->y < -MAX_VELOCITY) m_velocity->y = -MAX_VELOCITY;
	if (m_velocity->z < -MAX_VELOCITY) m_velocity->z = -MAX_VELOCITY;

	// 位置の更新
	pos->x += m_velocity->x;
	pos->y += m_velocity->y;
	pos->z += m_velocity->z;
	// 移動限界
	if (pos->x < 0.0f)
	{
		pos->x = 0.0f;
		m_force->x = 0.0f;
	}
	if (pos->y < 0.0f)
	{
		pos->y = 0.0f;
		m_force->y = 0.0f;
	}
	if (pos->z < 0.0f)
	{
		pos->z = 0.0f;
		m_force->z = 0.0f;
	}
	pTrans->m_pos = pos;

	// 抵抗で加速度を減らす	fDrag(0.0f ～ 1.0f)
	m_force->x *= (1.0f - m_drag->x);
	if (fabsf(m_force->x) < 0.01f) m_force->x = 0.0f;
	m_force->y *= (1.0f - m_drag->y);
	if (fabsf(m_force->y) < 0.01f) m_force->y = 0.0f;
	m_force->z *= (1.0f - m_drag->z);
	if (fabsf(m_force->z) < 0.01f) m_force->z = 0.0f;
	
}