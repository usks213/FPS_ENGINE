//==================================================================
//												GameObject.h
//	���[���h�ő��݂ł�����́@�Q�[���I�u�W�F�N�g
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/27	�Q�[���I�u�W�F�N�g�N���X�쐬
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include "../ECS/Entity/IEntity.h"


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	// �O��`
	class Transform;

	class GameObject : public IEntity
	{
	public:
		// �R���X�g���N�^
		explicit GameObject(EntityManager* entityManager);
		// �f�X�g���N�^
		virtual ~GameObject();
		
		// �I�u�W�F�N�g������
		void OnCreate() override;
		// �I�u�W�F�N�g�j����
		void OnDestroy() override;

		// �X�^�[�g
		virtual void Start() {};
		// �G���h
		virtual void End() {};

		// ���O�̐ݒ�
		void SetName(std::string name) { m_name = name; }
		// �^�O�̐ݒ�
		void SetTag(std::string tag) { m_tag = tag; }
		// ���O�̎擾
		const std::string& name() { return m_name; }
		// �^�O�̎擾
		const std::string& tag()  { return m_tag; }

		// �Q�[���I�u�W�F�N�g(���g)�擾
		const std::weak_ptr<GameObject>& gameObject() { return m_gameObject; }
		// �g�����X�t�H�[���擾
		const std::weak_ptr<Transform>& transform()   { return m_transform; }

		// �A�N�e�B�u�ݒ�
		void SetActive(bool bActive) { m_bActive = bActive; }
		// �A�N�e�B�u�擾
		bool GetActive()			 { return m_bActive; }

	protected:
		// ���O
		std::string m_name;
		// �^�O
		std::string m_tag;

		// �Q�[���I�u�W�F�N�g(���g)
		std::weak_ptr<GameObject> m_gameObject;
		// �g�����X�t�H�[��
		std::weak_ptr<Transform> m_transform;

		// �A�N�e�B�u
		bool m_bActive;
	};
}
