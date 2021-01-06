//==================================================================
//								SphereColliderData.h
//	�f�[�^�w���ŋ������蔻��R���|�[�l���g
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/06	�f�[�^�w���ō쐬
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include "../System/Math.h"
#include "../ECS/Component/IComponentData.h"
#include "../ECS/Component/IECSComponent.h"
#include "Collider.h"

//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	// �O��`
	class GameObject;
	class Transform;
	class SphereColliderData;

	class ECSSphereCollisionSystem;
	class SphereColliderData;
	// �^��
	using ECSSphereCollider = IECSComponent<SphereColliderData, ECSSphereCollisionSystem>;

	class SphereColliderData final : public IComponentData<SphereColliderData, ECSSphereCollisionSystem>
	{
		friend ECSSphereCollisionSystem;
	public:
		// �R���X�g���N�^
		SphereColliderData();
		// �f�X�g���N�^
		virtual ~SphereColliderData();

		// �I�u�W�F�N�g������
		void OnCreate() override;
		// �I�u�W�F�N�g�j����
		void OnDestroy() override;

		// ���b�Z�[�W��M
		void SendComponentMessage(const std::string& message, void* value) override;

		// ���S���W
		void SetCenter(Vector3 center) { m_bound.SetCenter(center); }
		// �����蔻��T�C�Y
		void SetRadius(float radius) { m_bound.SetSize(Vector3(2, 2, 2) * radius); }

		// ���C���R���C�_�[��
		void SetMain(bool bMain) { m_bMain = bMain; }

		// �Q�[���I�u�W�F�N�g�擾
		const std::weak_ptr<GameObject>& gameObject() { return m_gameObject; }
		// �g�����X�t�H�[���擾
		const std::weak_ptr<Transform>& transform() { return m_transform; }

	protected:
		// �o�E���h�{�����[��
		Bounds m_bound;
		// ���
		bool m_bOldState;
		bool m_bCurState;
		// �`��
		bool m_bMain;

		// �Q�[���I�u�W�F�N�g
		std::weak_ptr<GameObject> m_gameObject;
		// �g�����X�t�H�[��
		std::weak_ptr<Transform> m_transform;

	public:
		// ���Ƌ��̓����蔻��
		static bool SpheretoSphere(SphereColliderData* collider, SphereColliderData* other);
	};
}
