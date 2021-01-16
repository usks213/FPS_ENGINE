//==================================================================
//								FieldScript.cpp
//	�t�B�[���h
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/14	�t�B�[���h�N���v�g�N���X�쐬
//
//===================================================================

//====== �C���N���[�h�� ======

// ���g
#include "FieldScript.h"

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
void FieldScript::Start()
{
	const auto& renderer = gameObject().lock()->AddComponent<MeshRenderer>();
	renderer->MakePlane("plane", 20, 20, 500, 500, 1, 1);
	//renderer->SetDiffuseTexture("data/texture/grid.png");
	renderer->SetNormalTexture("data/texture/sampledNormals.jpg");
	renderer->SetAmbientTexture("data/texture/skydome.png");
	m_renderer = renderer;
	//renderer->SetTexSize({ 100, 100, 0 });
	//renderer->UpdateTexMatrix();
	gameObject().lock()->transform().lock()->m_pos->y = -1;
}

//========================================
//
//	�X�V��
//
//========================================
void FieldScript::Update()
{
}

//========================================
//
//	��X�V��
//
//========================================
void FieldScript::LateUpdate()
{
	const auto& target = m_target.lock();
	if (target)
	{
		transform().lock()->m_pos->x = target->transform().lock()->m_pos->x;
		transform().lock()->m_pos->z = target->transform().lock()->m_pos->z;
	}

	// �e�N�X�`�����W�̓���
	Vector3 pos = transform().lock()->m_pos;
	Vector3 texPos;
	texPos->x = pos->x / 500.0f;
	texPos->y = -pos->z / 500.0f;

	m_renderer.lock()->SetTexPos(texPos);
	m_renderer.lock()->UpdateTexMatrix();

}

//========================================
//
//	�I����
//
//========================================
void FieldScript::End()
{
}
