//==================================================================
//												ISystem.h
//	�V�X�e���x�[�X
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/22	�V�X�e���N���X�쐬
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include <list>
#include "../uniqueTypeID.h"

//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	// �O��`
	class World;
	class EntityManager;

	class ISystem 
	{
	public:
		// �R���X�g���N�^
		explicit ISystem(World* pWorld);
		// �f�X�g���N�^
		virtual ~ISystem();

		// ������
		virtual void OnCreate();
		// �X�V
		virtual void OnUpdate() {};
		// ��X�V
		virtual void OnLateUpdate() {};
		// �`��
		virtual void OnDraw() {};
		// �폜��
		virtual void OnDestroy();

		// �X�V�����̎��s�����擾
		int GetUpdateOrder() { return m_nUpdateOrder; }
		// �R���|�[�l���g�̃\�[�g
		virtual void SortComponentList() = 0;

		// ID�̓o�^
		template <class T>
		void SetTypeID() { m_nTypeID = CUniqueTypeID::Get<T>(); }
		// ID�̎擾
		int GetID() { return m_nTypeID; }

	protected:
		// �G���e�B�e�B�}�l�[�W���[�̎擾
		EntityManager* GetEntityManager();

		// �X�V�����̎��s����ݒ�
		void SetUpdateOrder(int nOrder) { m_nUpdateOrder = nOrder; }

	private:
		// �e�̃��[���h
		World* m_pWorld;
		// �X�V�����̎��s��
		int m_nUpdateOrder;

		// �^ID
		int m_nTypeID;
	};


	namespace UpdateOrder
	{
		enum EUpdateOrder
		{
			eScript = 0,
			eAnimation2D,
			eAnimation3D,
			eRigidbody,
			eCollision,
			eTransform,
			eRenderer,

			eMaxUpdateOrder,
		};
	}
}
