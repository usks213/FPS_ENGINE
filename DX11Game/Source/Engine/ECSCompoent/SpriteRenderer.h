//==================================================================
//								SpriteRenderer.h
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

#pragma once

//====== �C���N���[�h�� ======
#include "Renderer.h"
#include <map>
#include <string>
#include "../Renderer/Camera.h"


//===== �}�N����` =====


//===== �N���X��` =====

namespace ECS
{
	class SpriteRenderer : public Renderer
	{
	public:
		// ���C���[
		enum ESpriteLayer
		{
			// �擪
			eTop,

			// �t�F�[�h
			eFade3 = 1,
			eFade2,
			eFade1,

			// UI
			eUI5 = 5,
			eUI4,
			eUI3,
			eUI2,
			eUI1,

			eDefault,

			// �w�i
			eBG3 = (int)VIEW_FAR_Z + 1,
			eBG2,
			eBG1,

			// �Ō�
			eBottom,

			eMaxLayer,
		};
	public:
		// �R���X�g���N�^
		SpriteRenderer();
		// �f�X�g���N�^
		~SpriteRenderer();

		// �e�����_���[�̕`�揈��
		void EarlyDraw(ID3D11DeviceContext* pDC) override;
		void LateDraw(ID3D11DeviceContext* pDC) override;

		// �J��������̋������i�[
		virtual void LayerUpdate(){}

		//--- �}�e���A��
		// �J���[�ύX
		void SetDiffuseColor(Vector3 color) { m_color = color; }
		void SetDiffuseColor(float r, float g, float b) {
			m_color->x = r;
			m_color->y = g;
			m_color->z = b;
		}
		void SetDiffuseColor(float r, float g, float b, float a) {
			m_color->x = r;
			m_color->y = g;
			m_color->z = b;
			m_fAlpha = a;
		}
		void SetAlpha(float alpha) { m_fAlpha = alpha; }

		// �e�N�X�`���̃Z�b�g
		void SetDiffuseTexture(const char* filename);
		void SetDiffuseTexture(const wchar_t* filename);

		// ���C���[
		void SetLayer(ESpriteLayer eLayer) { m_fLayer = (float)eLayer; }
		void SetLayer(int nLayer) { m_fLayer = (float)nLayer; }


		// �J���[�擾
		Vector3 GetColor() { return m_color; }
		// �A���t�@�擾
		float GetAlpha() { return m_fAlpha; }
		// ���C���[�擾
		int GetLayer() { return (int)m_fLayer; }

	private:
		// �e�N�X�`��
		ID3D11ShaderResourceView* m_pTexture;
		// �J���[
		Vector3 m_color;
		// �A���t�@
		float m_fAlpha;
	};
}
