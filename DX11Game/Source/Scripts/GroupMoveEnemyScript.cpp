//==================================================================
//								GroupMoveEnemyScript.cpp
//	�O���[�v���[�u�G�l�~�[
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/15	�O���[�v���[�u�G�l�~�[�N���X�쐬
//
//===================================================================


//====== �C���N���[�h�� ======

// ���g
#include "GroupMoveEnemyScript.h"

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


// �l�[���X�y�[�X
using namespace ECS;


//===== �}�N����` =====



//******************** �X�N���v�g���\�b�h ********************


//========================================
//
//	�J�n��
//
//========================================
void GroupMoveEnemyScript::Start()
{
	// �x�[�X�N���X
	EnemyBaseScript::Start();

	// ���O�E�^�O
	gameObject().lock()->SetName("GroupMoveEnemy");

	//--- �R���|�[�l���̒ǉ�

	// �C���X�^���V���O�����_���[
	const auto& render = gameObject().lock()->AddComponent<InstancingMeshRenderer>();
	render->MakeS06("GroupMoveEnemy");
	render->SetDiffuseColor({ 150 / 255.0f,150 / 255.0f,150 / 255.0f,1 });
}

//========================================
//
//	�X�V��
//
//========================================
void GroupMoveEnemyScript::Update()
{
	// �x�[�X�N���X
	EnemyBaseScript::Update();
}

//========================================
//
//	��X�V��
//
//========================================
void GroupMoveEnemyScript::LateUpdate()
{
	// �x�[�X�N���X
	EnemyBaseScript::LateUpdate();
}

//========================================
//
//	�I����
//
//========================================
void GroupMoveEnemyScript::End()
{
	// �x�[�X�N���X
	EnemyBaseScript::End();
}
