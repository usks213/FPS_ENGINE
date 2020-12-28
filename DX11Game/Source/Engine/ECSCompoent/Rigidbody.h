//==================================================================
//								Rigidbody.h
//	���W�b�h�{�f�B
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/27	���W�b�h�{�f�B�N���X�쐬
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include "../System/Math.h"
#include "../ECS/Component/IComponent.h"


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	// �O��`
	class GameObject;
	class Transform;
	class Collider;

	class Rigidbody final : public IComponent
	{
		friend Collider;
	public:
		// �R���X�g���N�^
		Rigidbody();
		// �f�X�g���N�^
		~Rigidbody();

		// �I�u�W�F�N�g������
		void OnCreate() override;
		// �I�u�W�F�N�g�j����
		void OnDestroy() override;

		// ���b�Z�[�W��M
		void SendComponentMessage(const std::string& message, void* value) override;

		// �X�V����
		void Update();
		void LateUpdate();

		// �Q�[���I�u�W�F�N�g�擾
		const std::weak_ptr<GameObject>& gameObject() { return m_gameObject; }
		// �g�����X�t�H�[���擾
		const std::weak_ptr<Transform>& transform() { return m_transform; }

	private:
		// �����v�Z
		void CollisionPhysics(const std::shared_ptr<Rigidbody>& other, Vector3 normal);

		// �Q�[���I�u�W�F�N�g
		std::weak_ptr<GameObject> m_gameObject;
		// �g�����X�t�H�[��
		std::weak_ptr<Transform> m_transform;

	public:
		// �ړ�
		Vector3 GetForce() { return m_force; }
		void SetForce(Vector3 force) { m_force = force; }
		void SetForceX(float forceX) { m_force->x = forceX; }
		void SetForceY(float forceY) { m_force->y = forceY; }
		void SetForceZ(float forceZ) { m_force->z = forceZ; }
		void AddForce(Vector3 force) { m_force += force; }
		void AddForceX(float forceX) { m_force->x += forceX; }
		void AddForceY(float forceY) { m_force->y += forceY; }
		void AddForceZ(float forceZ) { m_force->z += forceZ; }
		// ��C��R
		Vector3 GetDrag() { return m_drag; }
		void SetDrag(Vector3 fDrag) { m_drag = fDrag; }
		void SetDragX(float fDragX) { m_drag->x = fDragX; }
		void SetDragY(float fDragY) { m_drag->y = fDragY; }
		void SetDragZ(float fDragZ) { m_drag->z = fDragZ; }

		// ��]
		Vector3 GetTorque() { return m_torque; }
		void SetTorque(Vector3 torque) { m_torque = torque; }
		void SetTorqueX(float torqueX) { m_torque->x = torqueX; }
		void SetTorqueY(float torqueY) { m_torque->y = torqueY; }
		void SetTorqueZ(float torqueZ) { m_torque->z = torqueZ; }
		void AddTorque(Vector3 torque) { m_torque += torque; }
		void AddTorqueX(float torqueX) { m_torque->x += torqueX; }
		void AddTorqueY(float torqueY) { m_torque->y += torqueY; }
		void AddTorqueZ(float torqueZ) { m_torque->z += torqueZ; }
		// ��]��R
		Vector3 GetTorqueDrag() { return m_torqueDrag; }
		void SetTorqueDrag(Vector3 fTorqueDrag) { m_torqueDrag = fTorqueDrag; }
		void SetTorqueDragX(float fTorqueDragX) { m_torqueDrag->x = fTorqueDragX; }
		void SetTorqueDragY(float fTorqueDragY) { m_torqueDrag->y = fTorqueDragY; }
		void SetTorqueDragZ(float fTorqueDragZ) { m_torqueDrag->z = fTorqueDragZ; }

		// �����g�p
		bool GetUsePhysics() { return m_bUsePhysics; }
		void SetUsePhysics(bool bUse) { m_bUsePhysics = bUse; }
		// �d�͎g�p
		bool GetUseGravity() { return m_bUseGravity; }
		void SetUseGravity(bool bUse) { m_bUseGravity = bUse; }
		// �d��
		Vector3 GetGravityForce() { return m_fGraviyForce; }
		void SetGravityForce(Vector3 fForce) { m_fGraviyForce = fForce; }
		// ����
		void SetMass(float fMass) { m_fMass = fMass; }
		// ���C
		void SetStaticFriction(float fFriction)  { m_fStaticFriction = fFriction; }
		void SetDynamicFriction(float fFriction) { m_fDynamicFriction = fFriction; }
		// �����W��
		void SetE(float e) { m_e = e; }
	private:
		// �ړ�
		Vector3 m_velocity;
		Vector3 m_force;
		Vector3 m_forceBackUp;
		Vector3 m_drag;
		// ��]
		Vector3 m_angularVelocity;
		Vector3 m_torque;
		Vector3 m_torqueDrag;

		// �����g�p
		bool m_bUsePhysics;

		// �d��
		bool m_bUseGravity;
		Vector3 m_fGraviyForce;
		// ����
		float m_fMass;
		// ���C
		float m_fStaticFriction;	// �Î~
		float m_fDynamicFriction;	// ��
		// �����W��
		float m_e;
	};
}
