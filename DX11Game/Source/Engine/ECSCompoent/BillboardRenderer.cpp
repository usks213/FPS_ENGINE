//==================================================================
//								BillboardRenderer.cpp
//	�r���{�[�h�����_���[
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
#include "BillboardRenderer.h"
#include "../ECSSystem/RendererSystem.h"
#include "../ECS/Entity/EntityManager.h"

#include "Transform.h"
#include "../Renderer/Camera.h"
#include "../System/Texture.h"

using namespace ECS;

//===== �}�N����` =====

//===== �O���[�o���ϐ� =====
MESH BillboardRenderer::m_masterMesh;


//========================================
//
//	�R���X�g���N�^
//
//========================================
BillboardRenderer::BillboardRenderer()
{
	m_material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_material.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_material.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_material.Emissive = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_material.Power = 0.0f;

	m_diffuseTexture = nullptr;
	m_eTranslucentType = eTranslucentType::TT_NOAFFECT;

	// ���b�V���̎擾
	m_mesh = GetBillboardMesh();
}

//========================================
//
//	�f�X�g���N�^
//
//========================================
BillboardRenderer::~BillboardRenderer()
{
}

//========================================
//
//	�O�`��
//
//========================================
void BillboardRenderer::EarlyDraw(ID3D11DeviceContext* pDC)
{
	const auto& mesh = m_mesh;

	if (mesh)
	{
		m_mesh->mtxTexture = m_mtxTexture;
		m_mesh->pMaterial = &m_material;
		m_mesh->pTexture = m_diffuseTexture;

		DrawMeshShadow(pDC, mesh);
	}
}

//========================================
//
//	��`��
//
//========================================
void BillboardRenderer::LateDraw(ID3D11DeviceContext* pDC)
{
	const auto& mesh = m_mesh;

	if (mesh)
	{
		// �}�g���b�N�X�쐬
		XMMATRIX mtxWorld, mtxScale, mtxTranslate;
		// �J�����s��擾
		XMFLOAT4X4& mtxView = CCamera::GetMainCamera()->GetViewMatrix();
		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();
		XMFLOAT4X4* m_mtxWorld = m_transform.lock()->GetWorldMatrix();
		XMStoreFloat4x4(m_mtxWorld, mtxWorld);

		m_mtxWorld->_11 = mtxView._11;
		m_mtxWorld->_12 = mtxView._21;
		m_mtxWorld->_13 = mtxView._31;
		m_mtxWorld->_21 = mtxView._12;
		m_mtxWorld->_22 = mtxView._22;
		m_mtxWorld->_23 = mtxView._32;
		m_mtxWorld->_31 = mtxView._13;
		m_mtxWorld->_32 = mtxView._23;
		m_mtxWorld->_33 = mtxView._33;

		mtxWorld = XMLoadFloat4x4(m_mtxWorld);

		// �X�P�[���𔽉f
		Vector3 scale = m_transform.lock()->m_scale;
		mtxScale = XMMatrixScaling(scale->x, scale->y, 1.0f);
		mtxWorld = XMMatrixMultiply(mtxScale, mtxWorld);

		// �ړ��𔽉f
		Vector3 pos = m_transform.lock()->m_pos;
		mtxTranslate = XMMatrixTranslation(pos->x, pos->y, pos->z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		XMStoreFloat4x4(m_mtxWorld, mtxWorld);

		m_mesh->mtxWorld = *m_mtxWorld;

		m_mesh->mtxTexture = m_mtxTexture;
		m_mesh->pMaterial = &m_material;
		m_mesh->pTexture = m_diffuseTexture;

		SetBlendState(m_eState);
		// Z�o�b�t�@����
		SetZBuffer(false);

		DrawMesh(pDC, mesh, m_eTranslucentType);

		SetBlendState(BS_NONE);

		// Z�o�b�t�@
		SetZBuffer(true);
	}
}

//========================================
//
// ���b�V���̎擾
//
//========================================
MESH* BillboardRenderer:: GetBillboardMesh()
{
	// ���b�V������������Ă���
	if (m_masterMesh.pVertexBuffer || m_masterMesh.pIndexBuffer ||
		m_masterMesh.nNumVertex || m_masterMesh.nNumIndex)
	{
		return &m_masterMesh;
	}

	// �I�u�W�F�N�g�̒��_�z��𐶐�
	m_masterMesh.nNumVertex = 4;
	VERTEX_3D* pVertexWk = new VERTEX_3D[m_masterMesh.nNumVertex];

	// ���_�z��̒��g�𖄂߂�
	VERTEX_3D* pVtx = pVertexWk;

	// ���_���W�̐ݒ�
	pVtx[0].vtx = XMFLOAT3(-1.0f / 2, -1.0f / 2, 0.0f);
	pVtx[1].vtx = XMFLOAT3(-1.0f / 2, 1.0f / 2, 0.0f);
	pVtx[2].vtx = XMFLOAT3(1.0f / 2, -1.0f / 2, 0.0f);
	pVtx[3].vtx = XMFLOAT3(1.0f / 2, 1.0f / 2, 0.0f);

	// �@���̐ݒ�
	pVtx[0].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVtx[1].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVtx[2].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVtx[3].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// ���ˌ��̐ݒ�
	pVtx[0].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// �e�N�X�`�����W�̐ݒ�
	pVtx[0].tex = XMFLOAT2(0.0f, 1.0f);
	pVtx[1].tex = XMFLOAT2(0.0f, 0.0f);
	pVtx[2].tex = XMFLOAT2(1.0f, 1.0f);
	pVtx[3].tex = XMFLOAT2(1.0f, 0.0f);

	// �C���f�b�N�X�z��𐶐�
	m_masterMesh.nNumIndex = 4;
	int* pIndexWk = new int[m_masterMesh.nNumIndex];

	// �C���f�b�N�X�z��̒��g�𖄂߂�
	pIndexWk[0] = 0;
	pIndexWk[1] = 1;
	pIndexWk[2] = 2;
	pIndexWk[3] = 3;

	ID3D11Device* pDevice = GetDevice();
	// ���_�o�b�t�@����
	HRESULT hr = MakeMeshVertex(pDevice, &m_masterMesh, pVertexWk, pIndexWk);

	// �ꎞ�z��̉��
	delete[] pIndexWk;
	delete[] pVertexWk;

	m_masterMesh.bLight = false;

	return &m_masterMesh;
}

//========================================
//
// �x�[�X�J���[�e�N�X�`���̃Z�b�g
//
//========================================
void BillboardRenderer::SetDiffuseTexture(const char* filename)
{
	m_diffuseTexture = CTexture::GetTexture(GetDevice(), filename);
}

//========================================
//
// �x�[�X�J���[�e�N�X�`���̃Z�b�g
//
//========================================
void BillboardRenderer::SetDiffuseTexture(const wchar_t* filename)
{
	m_diffuseTexture = CTexture::GetTexture(GetDevice(), filename);
}