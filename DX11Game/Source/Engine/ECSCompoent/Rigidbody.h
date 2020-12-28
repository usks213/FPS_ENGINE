//==================================================================
//								Rigidbody.h
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

#pragma once

//====== インクルード部 ======
#include "../System/Math.h"
#include "../ECS/Component/IComponent.h"


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	// 前定義
	class GameObject;
	class Transform;
	class Collider;

	class Rigidbody final : public IComponent
	{
		friend Collider;
	public:
		// コンストラクタ
		Rigidbody();
		// デストラクタ
		~Rigidbody();

		// オブジェクト生成時
		void OnCreate() override;
		// オブジェクト破棄時
		void OnDestroy() override;

		// メッセージ受信
		void SendComponentMessage(const std::string& message, void* value) override;

		// 更新処理
		void Update();
		void LateUpdate();

		// ゲームオブジェクト取得
		const std::weak_ptr<GameObject>& gameObject() { return m_gameObject; }
		// トランスフォーム取得
		const std::weak_ptr<Transform>& transform() { return m_transform; }

	private:
		// 物理計算
		void CollisionPhysics(const std::shared_ptr<Rigidbody>& other, Vector3 normal);

		// ゲームオブジェクト
		std::weak_ptr<GameObject> m_gameObject;
		// トランスフォーム
		std::weak_ptr<Transform> m_transform;

	public:
		// 移動
		Vector3 GetForce() { return m_force; }
		void SetForce(Vector3 force) { m_force = force; }
		void SetForceX(float forceX) { m_force->x = forceX; }
		void SetForceY(float forceY) { m_force->y = forceY; }
		void SetForceZ(float forceZ) { m_force->z = forceZ; }
		void AddForce(Vector3 force) { m_force += force; }
		void AddForceX(float forceX) { m_force->x += forceX; }
		void AddForceY(float forceY) { m_force->y += forceY; }
		void AddForceZ(float forceZ) { m_force->z += forceZ; }
		// 空気抵抗
		Vector3 GetDrag() { return m_drag; }
		void SetDrag(Vector3 fDrag) { m_drag = fDrag; }
		void SetDragX(float fDragX) { m_drag->x = fDragX; }
		void SetDragY(float fDragY) { m_drag->y = fDragY; }
		void SetDragZ(float fDragZ) { m_drag->z = fDragZ; }

		// 回転
		Vector3 GetTorque() { return m_torque; }
		void SetTorque(Vector3 torque) { m_torque = torque; }
		void SetTorqueX(float torqueX) { m_torque->x = torqueX; }
		void SetTorqueY(float torqueY) { m_torque->y = torqueY; }
		void SetTorqueZ(float torqueZ) { m_torque->z = torqueZ; }
		void AddTorque(Vector3 torque) { m_torque += torque; }
		void AddTorqueX(float torqueX) { m_torque->x += torqueX; }
		void AddTorqueY(float torqueY) { m_torque->y += torqueY; }
		void AddTorqueZ(float torqueZ) { m_torque->z += torqueZ; }
		// 回転抵抗
		Vector3 GetTorqueDrag() { return m_torqueDrag; }
		void SetTorqueDrag(Vector3 fTorqueDrag) { m_torqueDrag = fTorqueDrag; }
		void SetTorqueDragX(float fTorqueDragX) { m_torqueDrag->x = fTorqueDragX; }
		void SetTorqueDragY(float fTorqueDragY) { m_torqueDrag->y = fTorqueDragY; }
		void SetTorqueDragZ(float fTorqueDragZ) { m_torqueDrag->z = fTorqueDragZ; }

		// 物理使用
		bool GetUsePhysics() { return m_bUsePhysics; }
		void SetUsePhysics(bool bUse) { m_bUsePhysics = bUse; }
		// 重力使用
		bool GetUseGravity() { return m_bUseGravity; }
		void SetUseGravity(bool bUse) { m_bUseGravity = bUse; }
		// 重力
		Vector3 GetGravityForce() { return m_fGraviyForce; }
		void SetGravityForce(Vector3 fForce) { m_fGraviyForce = fForce; }
		// 質量
		void SetMass(float fMass) { m_fMass = fMass; }
		// 摩擦
		void SetStaticFriction(float fFriction)  { m_fStaticFriction = fFriction; }
		void SetDynamicFriction(float fFriction) { m_fDynamicFriction = fFriction; }
		// 反発係数
		void SetE(float e) { m_e = e; }
	private:
		// 移動
		Vector3 m_velocity;
		Vector3 m_force;
		Vector3 m_forceBackUp;
		Vector3 m_drag;
		// 回転
		Vector3 m_angularVelocity;
		Vector3 m_torque;
		Vector3 m_torqueDrag;

		// 物理使用
		bool m_bUsePhysics;

		// 重力
		bool m_bUseGravity;
		Vector3 m_fGraviyForce;
		// 質量
		float m_fMass;
		// 摩擦
		float m_fStaticFriction;	// 静止
		float m_fDynamicFriction;	// 動
		// 反発係数
		float m_e;
	};
}
