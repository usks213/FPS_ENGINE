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
#include "Transform.h"

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
		// �R���C�_�[�^�C�v
		virtual EColliderType GetColliderType() = 0;

		// ���S���W
		void SetCenter(Vector3 center) { m_bound.SetCenter(center); }

	protected:
		// �o�E���h�{�����[��
		Bounds m_bound;
		// �g���K�[
		bool m_bTriggr;
		// �g�����X�t�H�[��
		std::weak_ptr<Transform> m_trans;
		// ���
		bool m_bOldState;
		bool m_bCurState;
		
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
