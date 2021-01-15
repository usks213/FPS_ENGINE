//==================================================================
//								Renderer.h
//	�����_���[
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/23	ECS�p�ɍĐ݌v
//
//===================================================================


//====== �C���N���[�h�� ======
#include "Renderer.h"
#include "../ECSSystem/RendererSystem.h"
#include "../ECS/Entity/EntityManager.h"

#include "Transform.h"
#include "../main.h"
#include "../Renderer/Camera.h"

using namespace ECS;

//===== �}�N����` =====



//========================================
//
//	�R���X�g���N�^
//
//========================================
Renderer::Renderer()
{
	// �e�N�X�`�����W
	m_texPos = Vector3(0.0f, 0.0f, 0.0f);
	m_texSize = Vector3(1.0f, 1.0f, 1.0f);
	XMMATRIX mTex = XMMatrixScaling(m_texSize->x, m_texSize->y, 0.0f);
	mTex *= XMMatrixTranslation(m_texPos->x, m_texPos->y, 1.0f);
	XMStoreFloat4x4(&m_mtxTexture, mTex);

	// �u�����h�X�e�[�g
	m_eState = BS_NONE;

	m_bUpdateLayer = true;
}

//========================================
//
//	�f�X�g���N�^
//
//========================================
Renderer::~Renderer()
{

}

//========================================
//
//	������
//
//========================================
void Renderer::OnCreate()
{
	// �V�X�e���Ɋi�[
	RendererSystem* sys = GetEntityManager()->GetWorld()->GetSystem<RendererSystem>();
	if (sys) sys->AddList(this);

	// �g�����X�t�H�[������擾
	m_transform = m_Parent.lock()->GetComponent<Transform>();
}

//========================================
//
//	�j����
//
//========================================
void Renderer::OnDestroy()
{
	// �V�X�e�����珜�O
	RendererSystem* sys = GetEntityManager()->GetWorld()->GetSystem<RendererSystem>();
	if (sys) sys->RemoveList(this);
}


//========================================
//
//	�J��������̋������v�Z
//
//========================================
void Renderer::LayerUpdate()
{
	if (!m_bUpdateLayer) return;

	// �J�������W
	Vector3 cameraPos = CCamera::GetMainCamera()->GetPos();
	// �������i�[
	m_fLayer = Vector3::Length(m_transform.lock()->m_pos - cameraPos);
}

//========================================
//
// �e�N�X�`���}�g���b�N�X�̍X�V
//
//========================================
void Renderer::UpdateTexMatrix()
{
	// �e�N�X�`���}�g���b�N�X�X�V
	XMMATRIX mTex = XMMatrixScaling(m_texSize->x, m_texSize->y, 0.0f);
	mTex *= XMMatrixTranslation(m_texPos->x, m_texPos->y, 1.0f);
	XMStoreFloat4x4(&m_mtxTexture, mTex);
}