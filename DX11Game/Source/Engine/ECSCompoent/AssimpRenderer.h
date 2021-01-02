//==================================================================
//								AssimpRenderer.h
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

#pragma once

//====== �C���N���[�h�� ======
#include "Renderer.h"
#include <map>
#include "../Renderer/AssimpModel.h"
#include <string>


//===== �}�N����` =====


//===== �N���X��` =====

namespace ECS
{
	class AssimpRenderer : public Renderer
	{
	public:
		// �R���X�g���N�^
		AssimpRenderer();
		// �f�X�g���N�^
		~AssimpRenderer();

		// �e�����_���[�̕`�揈��
		void EarlyDraw(ID3D11DeviceContext* pDC) override;
		void LateDraw(ID3D11DeviceContext* pDC) override;

		// ���f���̃��[�h
		bool ModelLoad(std::string filename);
		// ���f���̉��
		void ModelRelese();
		
		//--- �}�e���A��
		TAssimpMaterial* GetMaterial();

		// �J���[�ύX
		void SetDiffuseColor(XMFLOAT4 color)
		{
			m_diffuseColor = color;
		}

		// �x�[�X�J���[�e�N�X�`���̃Z�b�g
		void SetDiffuseTexture(const char* filename);
		void SetDiffuseTexture(const wchar_t* filename);

		// �A�j���[�V�����C���f�b�N�X
		void SetAnimIndex(int nIndex)
		{
			if (m_model)
			{
				m_model->SetAnimIndex(nIndex);
			}
		}

		// ���݂̃A�j���[�V�����C���f�b�N�X���擾
		int GetCurrentAnimIndex()
		{
			if (m_model)
			{
				return m_model->GetCurrentAnimIndex();
			}
			return -1;
		}

		// �A�j���[�V�����X�s�[�h
		void SetAnimSpeed(double dSpeed)
		{
			if (m_model)
			{
				m_model->SetAnimSpeed(dSpeed);
			}
		}
		// �A�j���[�V�����̍Đ��ꏊ
		void SetStartAnimTime(double dTime)
		{
			if (m_model)
			{
				m_model->SetStartAnimTime(dTime);
			}
		}

		// �A�j���[�V�����̍Đ�
		void PlayAnim() {
			if (m_model) {
				m_model->SetStopAnim(false);
			}
		}
		// �A�j���[�V�����̃X�g�b�v
		void StopAnim() {
			if (m_model) {
				m_model->SetStopAnim(true);
			}
		}
	private:
		CAssimpModel* m_model;
		std::string m_filename;
		XMFLOAT4 m_diffuseColor;

		// AssimpModel�v�[��
		static std::map<const std::string, CAssimpModel*, std::less<>> m_assimpModelPool;
		// �}�e���A���v�[��
		static std::map<const CAssimpModel*, TAssimpMaterial*, std::less<>> m_materialPool;
	};
}
