//==================================================================
//								Script.h
//	�S�ẴX�N���v�g�R���|�[�l���g��
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/27	�X�N���v�g�x�[�X�N���X�쐬
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include "../System/Math.h"
#include "../ECS/Component/IComponent.h"
#include "../ECS/Entity/EntityManager.h"

//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	// �O��`
	class ScriptSystem;
	class GameObject;
	class Transform;
	class Collider;
	class DeltaCollider;
	class DeltaCollider;
	class Rigidbody;
	class Animator2D;

	class Script : public IComponent
	{
		friend ScriptSystem;
	public:
		// �R���X�g���N�^
		Script();
		// �f�X�g���N�^
		virtual ~Script();

		// �I�u�W�F�N�g������
		void OnCreate() override;
		// �I�u�W�F�N�g�j����
		void OnDestroy() override;

		// ���b�Z�[�W��M
		void SendComponentMessage(const std::string& message, void* value = nullptr) override;

		// �Q�[���I�u�W�F�N�g�擾
		const std::weak_ptr<GameObject>& gameObject() { return m_gameObject; }
		// �g�����X�t�H�[���擾
		const std::weak_ptr<Transform>& transform() { return m_transform; }

	public:
		//--- �Q�[���I�u�W�F�N�g�֘A

		// �Q�[���I�u�W�F�N�g����
		template<class T> 
		std::shared_ptr<T> Instantiate(Vector3 pos = { 0,0,0 }, Vector3 rot = { 0,0,0 }, Vector3 scale = { 0,0,0 });

		// �Q�[���I�u�W�F�N�g�̔j��
		void Destroy(const std::shared_ptr<IEntity>& obj) { Destroy(obj.get()); }
		void Destroy(IEntity* obj) { GetEntityManager()->DestroyEntity(obj); }

	protected:
		// �X�N���v�g�֐�
		virtual void Start(){}
		virtual void Update(){}
		virtual void LateUpdate(){}
		virtual void End(){}

		//--- �R�[���o�b�N�֐�

		// �����蔻��
		virtual void OnCollisionEnter(Collider* collider) {};
		virtual void OnCollisionStay(Collider* collider)  {};
		virtual void OnCollisionExit(Collider* collider)  {};

		// Delta�����蔻��
		virtual void OnDeltaCollisionEnter(DeltaCollider* collider) {};
		virtual void OnDeltaCollisionStay(DeltaCollider* collider) {};
		virtual void OnDeltaCollisionExit(DeltaCollider* collider) {};

		// �A�j���[�V����
		virtual void OnAnimationExit2D(Animator2D* animator) {};

	private:
		// �Q�[���I�u�W�F�N�g
		std::weak_ptr<GameObject> m_gameObject;
		// �g�����X�t�H�[��
		std::weak_ptr<Transform> m_transform;
	};


	// �Q�[���I�u�W�F�N�g����
	template<class T>
	std::shared_ptr<T> Script::Instantiate(Vector3 pos, Vector3 rot, Vector3 scale)
	{
		// �Q�[���I�u�W�F�N�g����
		const auto& obj = GetEntityManager()->CreateEntity<T>();
		// �g�����X�t�H�[���擾
		const std::shared_ptr<Transform>& trans = obj->GetComponent<Transform>();

		// �l����
		trans->m_pos = pos;
		trans->m_rot = rot;
		trans->m_scale = scale;

		return obj;
	}
}
