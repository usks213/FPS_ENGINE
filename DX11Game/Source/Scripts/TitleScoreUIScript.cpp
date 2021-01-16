//==================================================================
//								TitleScoreUIScript.h
//	�^�C�g���ł̃����\��UI
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/15	�^�C�g�����\��UI�N���X�쐬
//
//===================================================================


//====== �C���N���[�h�� ======

// ���g
#include "TitleScoreUIScript.h"

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


// �l�[���X�y�[�X
using namespace ECS;


//===== �}�N����` =====
#define ANIM_DELTA_SPLIT_X (10)  // �摜�̕�����(��)
#define ANIM_DELTA_SPLIT_Y (1)  // �摜�̕�����(�c)

#define DELTA_SIZE_X	(64 * SCREEN_SCALE_X)
#define DELTA_SIZE_Y	(64*2 * SCREEN_SCALE_Y)
#define DELTA_OFFSET_X	(DELTA_SIZE_X * 0.85f)
#define DELTA_OFFSET_Y	(DELTA_SIZE_Y * 0.85f)

#define MAX_DELTA (6)

#define BEST_SCORE_SIZE_X (2400 / 5 * SCREEN_SCALE_X)
#define BEST_SCORE_SIZE_Y (DELTA_SIZE_Y)

//******************** �X�N���v�g���\�b�h ********************

//========================================
//
//	�J�n��
//
//========================================
void TitleScoreUIData::Start()
{
	// �g�����X�t�H�[��
	transform().lock()->m_scale = Vector3{ DELTA_SIZE_X, DELTA_SIZE_Y, 1 };

	// �����_���[
	const auto& renderer = gameObject().lock()->AddComponent<SpriteRenderer>();
	renderer->SetDiffuseTexture("data/texture/DeltaNumber.png");
	renderer->SetLayer(renderer->eUI1);
	renderer->SetTexSize(Vector3{ 1.0f / (float)ANIM_DELTA_SPLIT_X, 1.0f / (float)ANIM_DELTA_SPLIT_Y , 0 });
	renderer->SetRendererBlendState(BS_ALPHABLEND);
	m_sprite = renderer;

	// �A�j���[�V�����i���o�[
	m_nAnimNo = 0;



}


//========================================
//
//	�J�n��
//
//========================================
void TitleScoreUIScript::Start()
{
	// �g�����X�t�H�[��
	transform().lock()->m_pos = Vector3{ SCREEN_CENTER_X - DELTA_OFFSET_X * (1),
		-SCREEN_CENTER_Y + DELTA_OFFSET_Y, 1 };
	transform().lock()->m_scale = Vector3{ DELTA_SIZE_X, DELTA_SIZE_Y, 1 };

	// �����_���[
	const auto& renderer = gameObject().lock()->AddComponent<SpriteRenderer>();
	renderer->SetDiffuseTexture("data/texture/DeltaLogo.png");
	renderer->SetLayer(renderer->eUI1);
	renderer->SetRendererBlendState(BS_ALPHABLEND);
	

	// �����f�[�^�̐���
	for (int i = 0; i < MAX_DELTA; i++)
	{
		Vector3 pos = transform().lock()->m_pos;
		pos->x -= (MAX_DELTA - i) * DELTA_OFFSET_X;

		const auto& obj = Instantiate<GameObject>(pos);
		m_data.push_back(obj->AddComponent<TitleScoreUIData>());
	}


	// BestScore
	Vector3 pos = transform().lock()->m_pos;
	pos->x -= (MAX_DELTA) * DELTA_OFFSET_X + BEST_SCORE_SIZE_X / 2 + DELTA_OFFSET_X / 2;
	const auto& best = Instantiate<GameObject>(pos);
	const auto& bestSpr = best->AddComponent<SpriteRenderer>();
	bestSpr->SetDiffuseTexture("data/texture/BestScore.png");
	bestSpr->SetLayer(bestSpr->eUI1);
	bestSpr->SetRendererBlendState(BS_ALPHABLEND);
	best->transform().lock()->m_scale = Vector3{ BEST_SCORE_SIZE_X, BEST_SCORE_SIZE_Y, 1 };

	// �X�R�A�̃��[�h
	FILE* fp;
	int nScore = 0;

	// �O��̃X�R�A��ǂݍ���
	fopen_s(&fp, "data/score.bin", "rb");

	if (fp)
	{
		fread(&nScore, sizeof(int), 1, fp);

		fclose(fp);
	}

	m_nScore = nScore;


	// ���F�̍X�V
	UpdateTexCoord();
}

//========================================
//
//	�X�V��
//
//========================================
void TitleScoreUIScript::Update()
{
}

//========================================
//
//	��X�V��
//
//========================================
void TitleScoreUIScript::LateUpdate()
{
	// ���F�̍X�V
	UpdateTexCoord();
}

//========================================
//
//	�I����
//
//========================================
void TitleScoreUIScript::End()
{
}

//========================================
//
//	UV�X�V
//
//========================================
void TitleScoreUIScript::UpdateTexCoord()
{
	int tmp = m_nScore;
	int nCnt = 0;
	// �����𐔂���
	while (tmp > 0)
	{
		tmp /= 10;
		nCnt++;
	}

	// UV�X�V
	tmp = m_nScore;
	for (int i = m_data.size() - 1; 0 <= i; i--)
	{
		const auto& data = m_data[i].lock();
		if (!data) continue;

		data->m_nAnimNo = tmp % 10;

		data->m_sprite.lock()->SetTexPos(
			Vector3{ data->m_sprite.lock()->GetTexSize()->x * (data->m_nAnimNo % ANIM_DELTA_SPLIT_X) ,
					 data->m_sprite.lock()->GetTexSize()->y * (data->m_nAnimNo / ANIM_DELTA_SPLIT_X),
					 0 });

		if (data->m_nAnimNo == 0 && (m_data.size() - i) > nCnt)
		{
			data->m_sprite.lock()->SetAlpha(0);
		}
		else
		{
			data->m_sprite.lock()->SetAlpha(1);
		}

		tmp /= 10;
	}
}