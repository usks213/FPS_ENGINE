//==================================================================
//								StraightMoveEnemyScript.cpp
//	�X�g���[�g���[�u�G�l�~�[
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/08	�X�g���[�g���[�u�G�l�~�[�N���X�쐬
//
//===================================================================


//====== �C���N���[�h�� ======

// ���g
#include "StraightMoveEnemyScript.h"

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
void StraightMoveEnemyScript::Start()
{
	// �x�[�X�N���X
	EnemyBaseScript::Start();

	// ���O�E�^�O
	gameObject().lock()->SetName("StraightMoveEnemy");

	//--- �R���|�[�l���̒ǉ�

	// �C���X�^���V���O�����_���[
	const auto& render = gameObject().lock()->AddComponent<InstancingMeshRenderer>();
	render->MakeN15("StraightMoveEnemy");
	render->SetDiffuseColor({ 1,1,0,1 });
}

//========================================
//
//	�X�V��
//
//========================================
void StraightMoveEnemyScript::Update()
{
	// �x�[�X�N���X
	EnemyBaseScript::Update();
}

//========================================
//
//	��X�V��
//
//========================================
void StraightMoveEnemyScript::LateUpdate()
{
	// �x�[�X�N���X
	EnemyBaseScript::LateUpdate();
}

//========================================
//
//	�I����
//
//========================================
void StraightMoveEnemyScript::End()
{
	// �x�[�X�N���X
	EnemyBaseScript::End();
}
