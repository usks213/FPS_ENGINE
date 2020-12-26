//==================================================================
//												GameObject.cpp
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


//===== �C���N���[�h�� =====
#include "GameObject.h"
#include "../ECSCompoent/Transform.h"


using namespace ECS;


//===== �}�N����` =====


//===== �v���g�^�C�v�錾 =====


//===== �O���[�o���ϐ� =====


//===================================
//
//	�R���X�g���N�^
//
//===================================
GameObject::GameObject(EntityManager* entityManager)
	: IEntity(entityManager),
	m_bActive(true)
{
}

//===================================
//
//	�f�X�g���N�^
//
//===================================
GameObject::~GameObject()
{
}

//===================================
//
//	�������R�[���o�b�N
//
//===================================
void GameObject::OnCreate()
{
	// ���g�̃X�}�[�g�|�C���^
	m_gameObject = std::static_pointer_cast<GameObject>(m_self.lock());

	// �g�����X�t�H�[���쐬
	m_transform = AddComponent<Transform>();
	// �Q�[���I�u�W�F�N�g���Z�b�g
	m_transform.lock()->SetGameObject(m_gameObject);

	// �X�^�[�g�֐�
	this->Start();
}

//===================================
//
//	�j�����R�[���o�b�N
//
//===================================
void GameObject::OnDestroy()
{
	// �G���h�֐�
	this->End();
}

