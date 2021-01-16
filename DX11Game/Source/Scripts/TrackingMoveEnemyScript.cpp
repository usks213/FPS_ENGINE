//==================================================================
//								TrackingMoveEnemyScript.cpp
//	�ǔ��^�G�l�~�[
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/08	�g���b�L���O���[�u�G�l�~�[�N���X�쐬
//
//===================================================================


//====== �C���N���[�h�� ======

// ���g
#include "TrackingMoveEnemyScript.h"

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
void TrackingMoveEnemyScript::Start()
{
	// �x�[�X�N���X
	EnemyBaseScript::Start();

	// ���O�E�^�O
	gameObject().lock()->SetName("TrackingMoveEnemy");


	//--- �R���|�[�l���̒ǉ�

	// �C���X�^���V���O�����_���[
	const auto& render = gameObject().lock()->AddComponent<InstancingMeshRenderer>();
	render->MakeDodecahedron("TrackingMoveEnemy");
	render->SetDiffuseColor({ 1,100 / 255.0f,0,1 });
}

//========================================
//
//	�X�V��
//
//========================================
void TrackingMoveEnemyScript::Update()
{
	// �x�[�X�N���X
	EnemyBaseScript::Update();
}

//========================================
//
//	��X�V��
//
//========================================
void TrackingMoveEnemyScript::LateUpdate()
{
	// �x�[�X�N���X
	EnemyBaseScript::LateUpdate();
}

//========================================
//
//	�I����
//
//========================================
void TrackingMoveEnemyScript::End()
{
	// �x�[�X�N���X
	EnemyBaseScript::End();
}

