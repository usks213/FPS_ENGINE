//==================================================================
//								BombCollisionScript.cpp
//	�{�������蔻��X�N���v�g
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/11	�{���R���W�����X�N���v�g�N���X�쐬
//
//===================================================================


//====== �C���N���[�h�� ======

// ���g
#include "BombCollisionScript.h"

// �V�X�e��
#include "../Engine/System/input.h"
#include "../Engine/System/Sound.h"

// �}�l�[�W���[
#include "../Engine/ECS/Entity/EntityManager.h"

// �G���e�B�e�B
#include "../Engine/ECSEntity/GameObject.h"

// �R���|�[�l���g
#include "../Engine/ECSCompoent/Transform.h"
#include "../Engine/ECSCompoent/MeshRenderer.h"
#include "../Engine/ECSCompoent/BillboardRenderer.h"
#include "../Engine/ECSCompoent/AssimpRenderer.h"
#include "../Engine/ECSCompoent/SpriteRenderer.h"
#include "../Engine/ECSCompoent/InstancingMeshRenderer.h"
#include "../Engine/ECSCompoent/Rigidbody.h"
#include "../Engine/ECSCompoent/BoxCollider.h"
#include "../Engine/ECSCompoent/SphereCollider.h"

// ECS�R���|�[�l���g
#include "../Engine/ECSCompoent/DeltaCollider.h"

// ECS�V�X�e��
#include "../Engine/ECSSystem/DeltaCollisionSystem.h"

// �X�N���v�g
#include "DropDeltaScript.h"


// �l�[���X�y�[�X
using namespace ECS;


//===== �}�N����` =====



//******************** �X�N���v�g���\�b�h ********************


//========================================
//
//	�J�n��
//
//========================================
void BombCollisionScript::Start()
{
	// ���O�E�^�O
	gameObject().lock()->SetName("Bullet");
	gameObject().lock()->SetTag("Bullet");

	// �傫��
	transform().lock()->m_scale = Vector3(2000, 2000, 2000);

	//--- �R���|�[�l���̒ǉ�

	// �C���X�^���V���O�����_���[
	//gameObject().lock()->AddComponent<InstancingMeshRenderer>()->MakeIcosahedron("Bomb");


	// ECS�R���C�_�[
	gameObject().lock()->AddComponent<DeltaCollider>();

	// ��������
	m_nExitTime = 10;
}

//========================================
//
//	�X�V��
//
//========================================
void BombCollisionScript::Update()
{
	m_nExitTime--;
	if (m_nExitTime < 0)
	{
		// ���g�̍폜
		GetEntityManager()->DestroyEntity(gameObject().lock());
	}
}

//========================================
//
//	��X�V��
//
//========================================
void BombCollisionScript::LateUpdate()
{
}

//========================================
//
//	�I����
//
//========================================
void BombCollisionScript::End()
{
}
