//==================================================================
//								AssimpRenderer.cpp
//	�A�V���v�����_���[
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
#include "AssimpRenderer.h"
#include "../ECSSystem/RendererSystem.h"
#include "../ECS/Entity/EntityManager.h"

#include "Transform.h"
#include "../System/Texture.h"

using namespace ECS;

//===== �}�N����` =====


//===== �O���[�o���ϐ� =====
std::map<const std::string, CAssimpModel*, std::less<>> AssimpRenderer::m_assimpModelPool;
std::map<const CAssimpModel*, TAssimpMaterial*, std::less<>> AssimpRenderer::m_materialPool;



//========================================
//
//	�R���X�g���N�^
//
//========================================
AssimpRenderer::AssimpRenderer()
{
	m_model = nullptr;
	m_filename = "\0";
	m_diffuseColor = { 1,1,1,1 };
}

//========================================
//
//	�f�X�g���N�^
//
//========================================
AssimpRenderer::~AssimpRenderer()
{
}

//========================================
//
//	�O�`��
//
//========================================
void AssimpRenderer::EarlyDraw(ID3D11DeviceContext* pDC)
{
	const auto& model = m_model;

	if (model)
	{
		model->Draw(pDC, *m_transform.lock()->GetWorldMatrix(), eNoAffect, true);
	}
}

//========================================
//
//	��`��
//
//========================================
void AssimpRenderer::LateDraw(ID3D11DeviceContext* pDC)
{
	const auto& model = m_model;

	if (model)
	{
		// �J���[�ύX
		TAssimpMaterial* pMat = m_model->GetMaterial();
		if (pMat)
		{
			pMat->Kd = m_diffuseColor;
		}

		// �`��
		bool bExitFlg = model->Draw(pDC, *m_transform.lock()->GetWorldMatrix(), eNoAffect, false);

		// �����ŃR�[���o�b�N�֐��̏����H
		if (bExitFlg)
		{
			// �R�[���o�b�N�֐�
			m_Parent.lock()->SendComponentMessage("OnAnimationExit3D", this);
		}
	}
}


//========================================
//
// �x�[�X�J���[�e�N�X�`���̃Z�b�g
//
//========================================
void AssimpRenderer::SetDiffuseTexture(const char* filename)
{
	// �}�e���A���擾
	auto pMat = GetMaterial();
	// �e�N�X�`��
	if (pMat)
		pMat->pTexture = CTexture::GetTexture(GetDevice(), filename);
}

//========================================
//
// �x�[�X�J���[�e�N�X�`���̃Z�b�g
//
//========================================
void AssimpRenderer::SetDiffuseTexture(const wchar_t* filename)
{
	// �}�e���A���擾
	auto pMat = GetMaterial();
	// �e�N�X�`��
	if (pMat)
		pMat->pTexture = CTexture::GetTexture(GetDevice(), filename);
}

//========================================
//
// ���f�����[�h
//
//========================================
bool AssimpRenderer::ModelLoad(std::string filename)
{
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pDC = GetDeviceContext();

	// �v�[�����猟��
	const auto& itr = m_assimpModelPool.find(filename);

	// ���Ƀv�[���ɂ�������
	if (m_assimpModelPool.end() != itr)
	{
		m_model = itr->second;
		m_filename = filename;
		return true;
	}

	// �Ȃ�������V������������
	const auto& am = new CAssimpModel();

	// ���f���̃��[�h
	bool b = am->Load(pDevice, pDC, filename);

	// �v�[���ɓo�^
	m_assimpModelPool.emplace(filename, am);

	// �i�[
	m_model = am;
	m_filename = filename;

	// ���C�g�ƃJ����
	m_model->SetLight(CLight::GetMainLight());
	m_model->SetCamera(CCamera::GetMainCamera());

	return b;
}

//========================================
//
// ���f�����
//
//========================================
void AssimpRenderer::ModelRelese()
{
	const auto& model = m_model;

	if (model)
	{
		model->Release();
	}

	// �v�[�����猟��
	const auto& itr = m_assimpModelPool.find(m_filename);

	// �v�[���ɂ�������
	if (m_assimpModelPool.end() != itr)
	{
		m_assimpModelPool.erase(itr);
	}

	if (m_model)
	{
		delete m_model;
		m_model = nullptr;
		m_filename = "\0";
	}
}

//========================================
//
// �}�e���A���擾
//
//========================================
TAssimpMaterial* AssimpRenderer::GetMaterial()
{
	// ���f�����Ȃ�������
	if (!m_model) return nullptr;

	// �}�e���A������������
	if (m_model->GetMaterial()) return m_model->GetMaterial();

	// �}�e���A������
	const auto& itr = m_materialPool.find(m_model);

	// �}�e���A�����v�[���ɂ�������
	if (m_materialPool.end() != itr)
	{
		return itr->second;
	}

	// �V�K�쐬
	auto pMat = new TAssimpMaterial();

	// ���f���Ɋi�[
	m_model->SetMaterial(pMat);

	// �v�[���Ɋi�[
	m_materialPool.emplace(m_model, pMat);

	return pMat;
}