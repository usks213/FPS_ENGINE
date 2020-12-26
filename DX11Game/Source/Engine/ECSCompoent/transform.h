//==================================================================
//								Transform.h
//	�g�����X�t�H�[��
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/07/22	�g�����X�t�H�[���N���X�̍쐬
//	2020/09/23	�R���|�[�l���g��
//	2020/12/23	ECS�p�ɍĐ݌v
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

	class Transform final : public IComponent
	{
	public:
		// �R���X�g���N�^
		Transform();
		// �f�X�g���N�^
		~Transform();

		// �I�u�W�F�N�g������
		void OnCreate() override;
		// �I�u�W�F�N�g�j����
		void OnDestroy() override;

		// ���b�Z�[�W��M
		void SendComponentMessage(const std::string& message, void* value) override;

		// �}�g���b�N�X�̍X�V
		void UpdateMatrix();

		// �Q�b�g�֐�
		XMFLOAT4X4* GetWorldMatrix() { return &m_mtxWorld; }
		// �Z�b�g�֐�
		void SetWorldMatrix(const XMFLOAT4X4& world) { m_mtxWorld = world; }

		// �X�e�[�^�X
		Vector3 m_pos;				// ���S���W
		Vector3 m_scale;			// �c���T�C�Y
		Vector3 m_rot;				// �I�u�W�F�N�g�̉�]�p�x

		// �Q�[���I�u�W�F�N�g�Z�b�g
		void SetGameObject(std::weak_ptr<GameObject>& gameObject) { m_gameObject = gameObject; }
		// �Q�[���I�u�W�F�N�g�̎擾
		const std::weak_ptr<GameObject>& gameObject()			  { return m_gameObject; }

	private:
		// �}�g���b�N�X
		XMFLOAT4X4 m_mtxWorld;

		// �e�̃Q�[���I�u�W�F�N�g
		std::weak_ptr<GameObject> m_gameObject;
	};
}
