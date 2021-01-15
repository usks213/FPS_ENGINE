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

	HRESULT Init(ID3D11Device* pDevice);
	void Uninit();
	void Draw(ID3D11DeviceContext* pDeviceContext);

	static PostProcessing* GetMain() { return m_pMain; }

	XMFLOAT4* GetColor() { return &m_col; }
public:
	ID3D11VertexShader* m_pVertexShader;		// ���_�V�F�[�_
	ID3D11PixelShader*	m_pPixelShader;			// �s�N�Z���V�F�[�_
	ID3D11InputLayout*	m_pInputLayout;			// ���_�t�H�[�}�b�g

	ID3D11Buffer*		m_pVertexConstantBuffer;		// ���_�V�F�[�_�[�萔�o�b�t�@
	ID3D11Buffer*		m_pPixelConstantBuffer;			// �s�N�Z���V�F�[�_�[�萔�o�b�t�@
	ID3D11SamplerState* m_pSamplerState;				// �e�N�X�`�� �T���v��
	ID3D11Buffer*		m_pVertexBuffer;				// ���_�o�b�t�@

	ID3D11RenderTargetView*		m_pRenderTargetView;	// �����_�[�^�[�Q�b�g
	ID3D11Texture2D*			m_pRenderTexture;		// �����_�[�e�N�X�`��
	ID3D11ShaderResourceView*	m_pTexureResourceView;	// �e�N�X�`�����\�[�X

	ID3D11DepthStencilView*		m_pDepthStencilView;		// Z�o�b�t�@
	ID3D11SamplerState*			m_pDepthSamplerState;		// �e�N�X�`�� �T���v��
	ID3D11Texture2D*			m_pDepthStencTexure;		// �V���h�E�}�b�v�e�N�X�`��
	ID3D11ShaderResourceView*	m_pDepthStencResourceView;	// �V���h�E�}�b�v���\�[�X

private:
	VERTEX_2D	m_vertexWk[NUM_VERTEX] = {};	// ���_���i�[���[�N

	XMFLOAT3	m_pos;			// �|���S���̈ړ���
	XMFLOAT3	m_rot;			// �|���S���̉�]��
	XMFLOAT3	m_siz;			// �|���S���̃T�C�Y
	XMFLOAT4	m_col;			// �|���S���̒��_�J���[
				
	XMFLOAT2	m_posTex;		// UV���W
	XMFLOAT2	m_sizTex;		// �e�N�X�`�����o�T�C�Y
				
	XMFLOAT4X4	m_mProj;				// �ˉe�ϊ��s��
	XMFLOAT4X4	m_mView;				// �r���[�ϊ��s��
	XMFLOAT4X4	m_mWorld;				// ���[���h�ϊ��s��
	XMFLOAT4X4	m_mTex;					// �e�N�X�`���ϊ��s��

private:
	HRESULT MakeVertex(ID3D11Device* pDevice);

	static PostProcessing* m_pMain;
};


