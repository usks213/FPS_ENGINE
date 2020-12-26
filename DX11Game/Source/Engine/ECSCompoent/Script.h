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


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	// �O��`
	class ScriptSystem;
	class GameObject;
	class Transform;
	class Collider;


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

		// �A�j���[�V����


	private:
		// �Q�[���I�u�W�F�N�g
		std::weak_ptr<GameObject> m_gameObject;
		// �g�����X�t�H�[��
		std::weak_ptr<Transform> m_transform;
	};
}
