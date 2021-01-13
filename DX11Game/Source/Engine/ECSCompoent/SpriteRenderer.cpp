//==================================================================
//								SpriteRenderer.cpp
//	�X�v���C�g�����_���[
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/02	ECS�p�ɍĐ݌v
//
//===================================================================


//====== �C���N���[�h�� ======
#include "SpriteRenderer.h"
#include "../ECSSystem/RendererSystem.h"
#include "../ECS/Entity/EntityManager.h"

#include "Transform.h"
#include "../Renderer/Camera.h"
#include "../System/Texture.h"
#include "../System/polygon.h"


using namespace ECS;


//===== �}�N����` =====


//===== �O���[�o���ϐ� =====


//========================================
//
//	�R���X�g���N�^
//
//========================================
SpriteRenderer::SpriteRenderer()
{
	m_fLayer = eDefault;
	m_pTexture = nullptr;
	// �F
	m_color = Vector3{ 1,1,1 };
	m_fAlpha = 1.0f;

	// �u�����h�X�e�[�g
	m_eState = BS_ALPHABLEND;
}

//========================================
//
//	�f�X�g���N�^
//
//========================================
SpriteRenderer::~SpriteRenderer()
{
}

//========================================
//
//	�O�`��
//
//========================================
void SpriteRenderer::EarlyDraw(ID3D11DeviceContext* pDC)
{
}

//========================================
//
//	��`��
//
//========================================
void SpriteRenderer::LateDraw(ID3D11DeviceContext* pDC)
{
	// ����
	const auto& trans = m_transform.lock();
	if (!trans) return;

	// Z�o�b�t�@����
	SetZBuffer(false);
	// �u�����h�X�e�[�g
	SetBlendState(m_eState);

	// ���W
	SetPolygonPos(trans->m_pos->x, trans->m_pos->y);
	// �p�x
	SetPolygonAngle(trans->m_rot->z);
	// �T�C�Y
	SetPolygonSize(trans->m_scale->x, trans->m_scale->y);
	// �J���[
	SetPolygonColor(m_color->x, m_color->y, m_color->z);
	// �A���t�@
	SetPolygonAlpha(m_fAlpha);
	// �e�N�X�`�����W
	SetPolygonUV(m_texPos->x, m_texPos->y);
	// �e�N�X�`���T�C�Y
	SetPolygonFrameSize(m_texSize->x, m_texSize->y);
	// �e�N�X�`��
	SetPolygonTexture(m_pTexture);

	// �`��
	DrawPolygon(pDC);

	// �u�����h�X�e�[�g
	SetBlendState(BS_NONE);
	// Z�o�b�t�@�L��
	SetZBuffer(true);
}


//========================================
//
// �x�[�X�J���[�e�N�X�`���̃Z�b�g
//
//========================================
void SpriteRenderer::SetDiffuseTexture(const char* filename)
{
	// �e�N�X�`���̃Z�b�g
	m_pTexture = CTexture::GetTexture(GetDevice(), filename);
}

//========================================
//
// �x�[�X�J���[�e�N�X�`���̃Z�b�g
//
//========================================
void SpriteRenderer::SetDiffuseTexture(const wchar_t* filename)
{
	// �e�N�X�`���̃Z�b�g
	m_pTexture = CTexture::GetTexture(GetDevice(), filename);
}