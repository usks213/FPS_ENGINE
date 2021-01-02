//==================================================================
//								BillboardRenderer.h
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

#pragma once

//====== �C���N���[�h�� ======
#include "Renderer.h"
#include "../Renderer/mesh.h"


//===== �}�N����` =====


//===== �N���X��` =====

namespace ECS
{
	class BillboardRenderer : public Renderer
	{
	public:
		// �R���X�g���N�^
		BillboardRenderer();
		// �f�X�g���N�^
		~BillboardRenderer();

		// �e�����_���[�̕`�揈��
		void EarlyDraw(ID3D11DeviceContext* pDC) override;
		void LateDraw(ID3D11DeviceContext* pDC) override;

		// �r���{�[�h���b�V���擾
		static MESH* GetBillboardMesh();

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
		// �e�N�X�`��
		ID3D11ShaderResourceView* m_diffuseTexture;

		// ���b�V��
		static MESH m_masterMesh;
	};
}
