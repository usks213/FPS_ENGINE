//======================================================================
//											PostProcessing.h
//	�|�X�g�G�t�F�N�g
//
//======================================================================
//	author :	AT12A 05 �F�����W�V
//======================================================================
//	�J������
//
//	2021/01/13	�쐬
//
//======================================================================
#pragma once


//====== �C���N���[�h�� ======
#include "../main.h"


//===== �N���X��` =====
class PostProcessing
{
public:
	PostProcessing();
	~PostProcessing();

	HRESULT Init();
	HRESULT Uninit();
	HRESULT Draw();

public:
	ID3D11VertexShader* m_pVertexShader;		// ���_�V�F�[�_
	ID3D11PixelShader*	m_pPixelShader;			// �s�N�Z���V�F�[�_
	ID3D11InputLayout*	m_pInputLayout;			// ���_�t�H�[�}�b�g


	ID3D11RenderTargetView*		m_pRenderTargetView;	// �����_�[�^�[�Q�b�g
	ID3D11SamplerState*			m_pSamplerState;		// �e�N�X�`�� �T���v��
	ID3D11Texture2D*			m_pRenderTexture;		// �����_�[�e�N�X�`��
	ID3D11ShaderResourceView*	m_pTexureResourceView;	// �e�N�X�`�����\�[�X

	ID3D11Buffer* m_pVertexConstantBuffer;		// ���_�萔�o�b�t�@
	ID3D11Buffer* m_pPixelConstantBuffer;		// ���_�萔�o�b�t�@
};


