//==================================================================
//								InstancingMeshRenderer.h
//	�C���X�^���V���O���b�V�������_���[
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/23	ECS�p�ɍĐ݌v
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include "Renderer.h"
#include <map>
#include "../Renderer/instancingMesh.h"
#include <string>


//===== �}�N����` =====


//===== �N���X��` =====

namespace ECS
{
	class InstancingMeshRenderer : public Renderer
	{
	public:
		// �R���X�g���N�^
		InstancingMeshRenderer();
		// �f�X�g���N�^
		~InstancingMeshRenderer();

		// �I�u�W�F�N�g������
		void OnCreate() override;
		// �I�u�W�F�N�g�j����
		void OnDestroy() override;

		// �e�����_���[�̕`�揈��
		void EarlyDraw(ID3D11DeviceContext* pDC) override;
		void LateDraw(ID3D11DeviceContext* pDC) override;

		// �J��������̋���
		void LayerUpdate() override {}

		// �X�v���C�g���b�V��
		HRESULT MakeSprite(const std::string tag);
		// ���ʃ��b�V���̐���
		HRESULT MakePlane(const std::string tag, int nNumBlockX, int nNumBlockZ, float fSizeBlockX, float fSizeBlockZ,
			float fTexSizeX = 1.0f, float fTexSizeZ = 1.0f);
		// �L���[�u
		HRESULT MakeCube(const std::string tag);
		// �X�t�B�A
		HRESULT MakeSphere(std::string tag,
			int nSegment = 36, float fTexSplit = 1.0f, XMFLOAT3 pos = { 0,0,0 });
		// �X�t�B�A
		HRESULT MakeSphere2(std::string tag,
			int nNumBlockX, int nNumBlockY, float fSize = 0.5f, 
			float fTexSizeX = 1.0f, float fTexSizeY = 1.0f, 
			float fPosX = 0.0f, float fPosY = 0.0f, float fPosZ = 0.0f);
		// ���l�ʑ�
		HRESULT MakeTetraheron(const std::string tag);
		// ���ʑ� Octahedron
		HRESULT MakeOctahedron(const std::string tag);
		// �\��ʑ�
		HRESULT MakeDodecahedron(const std::string tag);
		// ��\�ʑ� Icosahedron
		HRESULT MakeIcosahedron(const std::string tag);

		//  s08
		HRESULT MakeS08(const std::string tag);
		//  n15
		HRESULT MakeN15(const std::string tag);
		//  s06
		HRESULT MakeS06(const std::string tag);

		//--- �}�e���A��
		// �J���[�ύX
		void SetDiffuseColor(XMFLOAT4 color) { if (m_mesh) m_mesh->material.Diffuse = color; }
		// �r���{�[�h
		void SetBillboard(bool bFlg) { if (m_mesh) m_mesh->bBillboard = bFlg; }
		// �u�����h�X�e�[�g
		void SetBlendStateData(EBlendState eState) { if (m_mesh) m_mesh->blendState = eState; }

		// �x�[�X�J���[�e�N�X�`���̃Z�b�g
		void SetDiffuseTexture(const char* filename);
		// �x�[�X�J���[�e�N�X�`���̃Z�b�g
		void SetDiffuseTexture(const wchar_t* filename);

		// �v���~�e�B�u
		void SetPrimitiveType(ePrimitiveType eType) { if (m_mesh) m_mesh->primitiveType = eType; }

	private:
		// ���b�V���f�[�^�̐���
		bool CreateMeshData(std::string tag);

	private:
		// ���b�V���f�[�^
		InstancingMesh* m_mesh;
		// �C���X�^���V���O�f�[�^
		InstancingMeshData m_data;

		// ���b�V���̃v�[��
		static std::map<const std::string, InstancingMesh*, std::less<>> m_meshPool;
		static std::map<const std::string, std::vector<InstancingMeshData*>, std::less<>> m_meshList;

		// ���b�V���^�O
		std::string m_tag;
	};
}
