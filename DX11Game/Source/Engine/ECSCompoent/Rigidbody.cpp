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
#include "Collider.h"


using namespace ECS;

//===== マクロ定義 =====
#define GRAVITY (9.8f / 60.0f)
#define DRAG (1.0f)

//#define MAX_VELOCITY (50.0f)


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
	m_fStaticFriction	= 0.0f;	// 静止
	m_fDynamicFriction	= 0.0f;	// 動
	// 反発係数
	m_e					= 1.0f;
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
	if (m_bUseGravity && m_bUsePhysics)
	{
		m_force += m_fGraviyForce * m_fMass;
	}

	// 外力
	m_velocity->x += m_force->x;
	m_velocity->y += m_force->y;
	m_velocity->z += m_force->z;

	// 速度限界
	//if (m_velocity->x > MAX_VELOCITY) m_velocity->x = MAX_VELOCITY;
	//if (m_velocity->y > MAX_VELOCITY) m_velocity->y = MAX_VELOCITY;
	//if (m_velocity->z > MAX_VELOCITY) m_velocity->z = MAX_VELOCITY;
	//if (m_velocity->x < -MAX_VELOCITY) m_velocity->x = -MAX_VELOCITY;
	//if (m_velocity->y < -MAX_VELOCITY) m_velocity->y = -MAX_VELOCITY;
	//if (m_velocity->z < -MAX_VELOCITY) m_velocity->z = -MAX_VELOCITY;

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
	// 更新
	pTrans->m_pos = pos;
}

//========================================
//
//	後更新
//
//========================================
void Rigidbody::LateUpdate()
{
	// 初期化
	m_forceBackUp = Vector3{ 0,0,0 };

	// 抵抗で加速度を減らす	fDrag(0.0f ～ 1.0f)
	m_force->x *= (1.0f - m_drag->x);
	if (fabsf(m_force->x) < 0.01f) m_force->x = 0.0f;
	m_force->y *= (1.0f - m_drag->y);
	if (fabsf(m_force->y) < 0.01f) m_force->y = 0.0f;
	m_force->z *= (1.0f - m_drag->z);
	if (fabsf(m_force->z) < 0.01f) m_force->z = 0.0f;
}

// 物理計算
void Rigidbody::CollisionPhysics(const std::shared_ptr<Rigidbody>& other, Vector3 normal)
{
	// リジッドボディ取得
	const auto& rb1 = this;
	const auto& rb2 = other;
	if (!rb1 || !rb2) return;
	
	// ベクトルの大きさ
	float magnitude = rb1->m_velocity.magnitude();
	// 壁ずりベクトル
	Vector3 scratch = Vector3::WallScratchVector(rb1->m_velocity, normal);
	// 垂直ベクトル
	Vector3 vertical = Vector3::WallVerticalVector(rb1->m_velocity, normal);
	Vector3 vertical2 = Vector3::WallVerticalVector(rb2->m_velocity, normal);


	//--- 垂直方向(反発)
	//float m1 = -rb2->m_fMass / (rb1->m_fMass + rb2->m_fMass);
	//float m2 =  rb1->m_fMass / (rb1->m_fMass + rb2->m_fMass);
	//Vector3 c = rb2->transform().lock()->m_pos - rb1->transform().lock()->m_pos;
	//float dot = Vector3::Dot((vertical - vertical2), c);
	float e = (rb1->m_e * rb2->m_e);

	//// 反発後の垂直速度
	//Vector3 verticalVector = vertical + c * dot * e * m1;
	//Vector3 verticalVector2 = vertical2 + c * dot * e * m2;

	// 反発後の垂直速度
	Vector3 verticalVector = 
		(vertical2 - vertical) * (1 + e) / (rb1->m_fMass / rb2->m_fMass + 1) + vertical;
	// 反発後の垂直速度(相手側)
	Vector3 verticalVector2 =
		(vertical - vertical2) * (1 + e) / (rb2->m_fMass / rb1->m_fMass + 1) + vertical2;


	//--- 水平方向(摩擦)
	// 水平方向の力
	Vector3 horizontalVector;
	Vector3 horizontalVector2;
	// 垂直抗力
	Vector3 N = vertical;
	// 静止摩擦
	float myu_s = rb1->m_fStaticFriction * rb2->m_fStaticFriction;
	// 動摩擦
	float myu_d = rb1->m_fDynamicFriction * rb2->m_fDynamicFriction;
	// 動摩擦力
	float F_d = myu_d * N.magnitude();
	if (F_d > 1.0f) F_d = 1.0f;

	// 最大静止摩擦力より大きいか
	if (scratch.magnitude() > myu_s * N.magnitude())
	{
		// 水平方向の力
		horizontalVector = scratch - scratch * F_d;
	}
	// 最大静止摩擦力より大きいか(相手側)
	if (F_d > myu_s * vertical2.magnitude())
	{
		// 水平方向の力
		horizontalVector2 = scratch * F_d;
	}


	//--- 力の合成
	// 物理をするか
	if (rb1->m_bUsePhysics)
	{
		//--- ベクトルの合成
		Vector3 force = horizontalVector + verticalVector;

		// 最終的な力
		//if (force.magnitude() > rb1->m_forceBackUp.magnitude())
		{
			rb1->m_force = force;
			rb1->m_forceBackUp = force;
			rb1->m_velocity = force;
		}
	}
	// 物理をするか(相手側)
	if (rb2->m_bUsePhysics)
	{
		//--- ベクトルの合成
		Vector3 force = horizontalVector2 + verticalVector2;

		// 最終的な力
		//if (force.magnitude() > rb2->m_forceBackUp.magnitude())
		{
			rb2->m_force = force;
			rb2->m_forceBackUp = force;
			rb2->m_velocity = force;
		}
	}
}