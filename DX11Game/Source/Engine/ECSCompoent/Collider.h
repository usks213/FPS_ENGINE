//==================================================================
//								Collider.h
//	�R���C�_�[�x�[�X
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/24	�R���C�_�[�x�[�X�N���X�쐬
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include "../System/Math.h"
#include "../ECS/Component/IComponent.h"
#include <vector>


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class Bounds
	{
	private:
		Vector3 m_center;
		Vector3 m_size;
	public:
		// �R���X�g���N�^
		Bounds(Vector3 center, Vector3 size);

		// �Q�b�g�֐�
		Vector3 GetCenter()		{ return m_center; }
		Vector3 GetHalfSize()	{ return m_size * 0.5f; }
		Vector3 GetMax()		{ return m_center + GetHalfSize(); }
		Vector3 GetMin()		{ return m_center - GetHalfSize(); }
		Vector3 GetSize()		{ return m_size; }
		float   GetRadius()		{ return GetHalfSize()->x; }

		// �Z�b�g�֐�
		void SetCenter(Vector3 center)	{ m_center = center; }
		void SetSize(Vector3 size)		{ m_size = size; }
	};

	// �O��`
	class CollisionSystem;
	class GameObject;
	class Transform;
	class Rigidbody;

	class Collider : public IComponent
	{
		friend CollisionSystem;
	public:
		enum EColliderType
		{
			eBox,
			eSphere,

			eMaxColliderType,
		};

	public:
		// �R���X�g���N�^
		Collider();
		// �f�X�g���N�^
		virtual ~Collider();

		// �I�u�W�F�N�g������
		void OnCreate() override;
		// �I�u�W�F�N�g�j����
		void OnDestroy() override;

		// ���b�Z�[�W��M
		void SendComponentMessage(const std::string& message, void* value) override;

		// �e�R���C�_�[�̓����蔻��
		virtual bool Judgment(Collider* other) = 0;
		const EColliderType& GetColliderType() { return m_eType; }

		// ���S���W
		void SetCenter(Vector3 center) { m_bound.SetCenter(center); }

		// �Q�[���I�u�W�F�N�g�擾
		const std::weak_ptr<GameObject>& gameObject() { return m_gameObject; }
		// �g�����X�t�H�[���擾
		const std::weak_ptr<Transform>& transform() { return m_transform; }
		// ���W�b�h�{�f�B�擾
		const std::weak_ptr<Rigidbody>& rigidbody() { return m_rigidbody; }

	protected:
		// �o�E���h�{�����[��
		Bounds m_bound;
		// �g���K�[
		bool m_bTriggr;
		// ���
		bool m_bOldState;
		bool m_bCurState;
		// �`��
		EColliderType m_eType;

		// �Q�[���I�u�W�F�N�g
		std::weak_ptr<GameObject> m_gameObject;
		// �g�����X�t�H�[��
		std::weak_ptr<Transform> m_transform;
		// ���W�b�h�{�f�B
		std::weak_ptr<Rigidbody> m_rigidbody;

		// ���ɓ����������X�g
		std::vector<Collider*> m_aIsHit;
		
	//=== �e�����蔻�� ===
	public:
		// AABBtoAABB ������p
		static bool AABBtoAABB(Collider* collider, Collider* other);
	protected:
		// �ڍה���@�����o������
		static bool BoxToBox(Collider* collider, Collider* other);
		static bool BoxToSphere(Collider* collider, Collider* other);
		static bool SphereToBox(Collider* collider, Collider* other);
		static bool SphereToSphere(Collider* collider, Collider* other);
	};


}
