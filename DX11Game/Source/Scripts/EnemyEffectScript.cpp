//==================================================================
//								EnemyEffectScript.cpp
//	�G�l�~�[�G�t�F�N�g
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/15	�G�l�~�[�G�t�F�N�g�N���X�쐬
//
//===================================================================


//====== �C���N���[�h�� ======

// ���g
#include "EnemyEffectScript.h"

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
#include "../Engine/ECSCompoent/Animator2D.h"
#include "../Engine/ECSCompoent/Animation2D.h"

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
void EnemyEffectScript::Start()
{
	// �X�P�[��
	transform().lock()->m_scale = Vector3{ 400,400,400 };

	// �r���{�[�h�����_���[
	const auto& bill = gameObject().lock()->AddComponent<BillboardRenderer>();
	bill->SetDiffuseTexture("data/texture/tktk_Darkness_5.png");
	bill->SetRendererBlendState(BS_ADDITIVE);

	// �A�j���[�V����
	const auto& anim = gameObject().lock()->AddComponent<Animator2D>();
	anim->SetSplit(5, 2);
	anim->AddAnimationDefault("Effect", 5 * 2);
	anim->SetAnimationSpeed(2);

	anim->SetAnimation("Effect");

	bill->UpdateTexMatrix();
}

//========================================
//
//	�X�V��
//
//========================================
void EnemyEffectScript::Update()
{
}

//========================================
//
//	��X�V��
//
//========================================
void EnemyEffectScript::LateUpdate()
{
}

//========================================
//
//	�I����
//
//========================================
void EnemyEffectScript::End()
{
}

//========================================
//
//	�A�j���[�V�����I�����R�[���o�b�N�֐�
//
//========================================
void EnemyEffectScript::OnAnimationExit2D(Animator2D* animator)
{
	if (animator->GetCurrentAnimation().lock()->GetName() == "Effect")
	{
		// ���g�̔j��
		Destroy(gameObject().lock());
	}
}