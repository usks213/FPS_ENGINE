//==================================================================
//								ScriptBaseLayoutBaseLayout.cpp
//	�X�N���v�g�x�[�X���C�A�E�g
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/27	�X�N���v�g�x�[�X���C�A�E�g�N���X�쐬
//
//===================================================================


//====== �C���N���[�h�� ======

// ���g
#include "ScriptBaseLayout.h"

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
void ScriptBaseLayout::Start()
{
}

//========================================
//
//	�X�V��
//
//========================================
void ScriptBaseLayout::Update()
{
}

//========================================
//
//	��X�V��
//
//========================================
void ScriptBaseLayout::LateUpdate()
{
}

//========================================
//
//	�I����
//
//========================================
void ScriptBaseLayout::End()
{
}


//******************** �R�[���o�b�N�֐� ********************


//========================================
//
// ����������
//
//========================================
void ScriptBaseLayout::OnCollisionEnter(Collider* collider)
{

}

//========================================
//
// �������Ă����
//
//========================================
void ScriptBaseLayout::OnCollisionStay(Collider* collider)
{

}

//========================================
//
// ���ꂽ��
//
//========================================
void ScriptBaseLayout::OnCollisionExit(Collider* collider)
{

}


//===== ECS =====

//========================================
//
// ����������
//
//========================================
void ScriptBaseLayout::OnDeltaCollisionEnter(DeltaCollider* collider)
{
}

//========================================
//
// �������Ă����
//
//========================================
void ScriptBaseLayout::OnDeltaCollisionStay(DeltaCollider* collider)
{
}

//========================================
//
// ���ꂽ��
//
//========================================
void ScriptBaseLayout::OnDeltaCollisionExit(DeltaCollider* collider)
{
}

