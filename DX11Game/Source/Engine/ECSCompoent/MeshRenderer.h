//==================================================================
//								MeshRenderer.h
//	���b�V�������_���[
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
#include "../Renderer/mesh.h"
#include <string>


//===== �}�N����` =====


//===== �N���X��` =====

namespace ECS
{
	class MeshRenderer : public Renderer
	{
	public:
		// �R���X�g���N�^
		MeshRenderer();
		// �f�X�g���N�^
		~MeshRenderer();

		// �e�����_���[�̕`�揈��
		void EarlyDraw(ID3D11DeviceContext* pDC) override;
		void LateDraw(ID3D11DeviceContext* pDC) override;

		// ���ʃ��b�V���̐���
		HRESULT MakePlane(const std::string tag, int nNumBlockX, int nNumBlockZ, float fSizeBlockX, float fSizeBlockZ,
			float fTexSizeX = 1.0f, float fTexSizeZ = 1.0f);
		// �L���[�u
		HRESULT MakeCube(const std::string tag);
		// �X�t�B�A
		HRESULT MakeSphere(std::string tag,
			int nSegment, float fScale, float fTexSplit = 1.0f, XMFLOAT3 pos = { 0,0,0 });

		//--- �}�e���A��
		// �J���[�ύX
		void SetDiffuseColor(XMFLOAT4 color) { m_material.Diffuse = color; }

		// �x�[�X�J���[�e�N�X�`���̃Z�b�g
		void SetDiffuseTexture(const char* filename);
		// �x�[�X�J���[�e�N�X�`���̃Z�b�g
		void SetDiffuseTexture(const wchar_t* filename);

		// �`��^�C�v
		void SetTranslucentType(eTranslucentType eType) { m_eTranslucentType = eType; }
		// �v���~�e�B�u
		void SetPrimitiveType(ePrimitiveType eType) { if (m_mesh) m_mesh->primitiveType = eType; }

	private:
		// ���b�V���f�[�^
		MESH* m_mesh;
		// �}�e���A���f�[�^
		MATERIAL m_material;

		// �`��^�C�v
		eTranslucentType m_eTranslucentType;

		// ���b�V���̃v�[��
		static std::map<const std::string, MESH*, std::less<>> m_meshPool;
	};
}