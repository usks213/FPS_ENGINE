//=============================================================================
//
// �|���S������ [polygon.cpp]
// Author : KOSHI USAMI
//
//=============================================================================
#include "polygon.h"
#include "Shader.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
// (�Ȃ�)

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
// �V�F�[�_�ɓn���l
struct SHADER_GLOBAL {
	XMMATRIX	mWorld;		// ���[���h�ϊ��s��(�]�u�s��)
	XMMATRIX	mView;		// �r���[�ϊ��s��(�]�u�s��)
	XMMATRIX	mProj;		// �ˉe�ϊ��s��(�]�u�s��)
	XMMATRIX	mTex;		// �e�N�X�`���ϊ��s��(�]�u�s��)
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
static HRESULT MakeVertexPolygon(ID3D11Device* pDevice);
static void SetVertexPolygon(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11ShaderResourceView*	g_pTexture;				// �e�N�X�`���ւ̃|�C���^

static VERTEX_2D					g_vertexWk[NUM_VERTEX];	// ���_���i�[���[�N

static XMFLOAT3						g_posPolygon;			// �|���S���̈ړ���
static XMFLOAT3						g_rotPolygon;			// �|���S���̉�]��
static XMFLOAT3						g_sizPolygon;			// �|���S���̃T�C�Y
static XMFLOAT4						g_colPolygon;			// �|���S���̒��_�J���[
static bool							g_bInvalidate;			// ���_�f�[�^�X�V�t���O

static XMFLOAT2						g_posTexFrame;			// UV���W
static XMFLOAT2						g_sizTexFrame;			// �e�N�X�`�����o�T�C�Y

static ID3D11Buffer*				g_pConstantBuffer;		// �萔�o�b�t�@
static ID3D11Buffer*				g_pVertexBuffer;		// ���_�o�b�t�@
static ID3D11SamplerState*			g_pSamplerState;		// �e�N�X�`�� �T���v��
static ID3D11VertexShader*			g_pVertexShader;		// ���_�V�F�[�_
static ID3D11InputLayout*			g_pInputLayout;			// ���_�t�H�[�}�b�g
static ID3D11PixelShader*			g_pPixelShader;			// �s�N�Z���V�F�[�_

static XMFLOAT4X4					g_mProj;				// �ˉe�ϊ��s��
static XMFLOAT4X4					g_mView;				// �r���[�ϊ��s��
static XMFLOAT4X4					g_mWorld;				// ���[���h�ϊ��s��
static XMFLOAT4X4					g_mTex;					// �e�N�X�`���ϊ��s��

//=============================================================================
// ����������
//=============================================================================
HRESULT InitPolygon(ID3D11Device* pDevice)
{
	HRESULT hr = S_OK;

	// �V�F�[�_������
	static const D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	hr = LoadShader("Vertex2D", "Pixel2D",
		&g_pVertexShader, &g_pInputLayout, &g_pPixelShader, layout, _countof(layout));
	if (FAILED(hr)) {
		return hr;
	}

	// �萔�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SHADER_GLOBAL);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = pDevice->CreateBuffer(&bd, nullptr, &g_pConstantBuffer);
	if (FAILED(hr)) return hr;

	// �e�N�X�`�� �T���v������
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	// ���[�v����
	//sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	//sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	//sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	// ���[�v�Ȃ�
	sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	hr = pDevice->CreateSamplerState(&sd, &g_pSamplerState);
	if (FAILED(hr)) {
		return hr;
	}

	// �ϊ��s�񏉊���
	XMStoreFloat4x4(&g_mWorld, XMMatrixIdentity());
	XMStoreFloat4x4(&g_mView, XMMatrixLookAtLH(XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));
	XMStoreFloat4x4(&g_mProj,
		XMMatrixOrthographicLH(SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f, 100.0f));
	XMStoreFloat4x4(&g_mTex, XMMatrixIdentity());
	g_mTex._44 = 0.0f;

	g_posPolygon = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_rotPolygon = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_sizPolygon = XMFLOAT3(1.0f, 1.0f, 1.0f);
	g_colPolygon = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	g_bInvalidate = false;

	g_posTexFrame = XMFLOAT2(0.0f, 0.0f);
	g_sizTexFrame = XMFLOAT2(1.0f, 1.0f);

	// ���_���̍쐬
	hr = MakeVertexPolygon(pDevice);

	return hr;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPolygon(void)
{
	// �e�N�X�`�� �T���v���̊J��
	SAFE_RELEASE(g_pSamplerState);
	// ���_�o�b�t�@�̉��
	SAFE_RELEASE(g_pVertexBuffer);
	// �萔�o�b�t�@�̉��
	SAFE_RELEASE(g_pConstantBuffer);
	// �s�N�Z���V�F�[�_���
	SAFE_RELEASE(g_pPixelShader);
	// ���_�t�H�[�}�b�g���
	SAFE_RELEASE(g_pInputLayout);
	// ���_�V�F�[�_���
	SAFE_RELEASE(g_pVertexShader);
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdatePolygon(void)
{
	// (�������Ȃ�)
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPolygon(ID3D11DeviceContext* pDeviceContext)
{
	// �g�k
	XMMATRIX mWorld = XMMatrixScaling(g_sizPolygon.x, g_sizPolygon.y, g_sizPolygon.z);
	// ��]
	mWorld *= XMMatrixRotationRollPitchYaw(XMConvertToRadians(g_rotPolygon.x),
		XMConvertToRadians(g_rotPolygon.y), XMConvertToRadians(g_rotPolygon.z));
	// �ړ�
	mWorld *= XMMatrixTranslation(g_posPolygon.x, g_posPolygon.y, g_posPolygon.z);
	// ���[���h �}�g���b�N�X�ɐݒ�
	XMStoreFloat4x4(&g_mWorld, mWorld);

	if (g_pTexture) {
		// �g�k
		mWorld = XMMatrixScaling(g_sizTexFrame.x, g_sizTexFrame.y, 1.0f);
		// �ړ�
		mWorld *= XMMatrixTranslation(g_posTexFrame.x, g_posTexFrame.y, 0.0f);
		// �e�N�X�`�� �}�g���b�N�X�ɐݒ�
		XMStoreFloat4x4(&g_mTex, mWorld);
	} else {
		// �e�N�X�`������
		g_mTex._44 = 0.0f;
	}

	// ���_�o�b�t�@�X�V
	SetVertexPolygon();

	pDeviceContext->VSSetShader(g_pVertexShader, nullptr, 0);
	pDeviceContext->PSSetShader(g_pPixelShader, nullptr, 0);
	pDeviceContext->IASetInputLayout(g_pInputLayout);

	UINT stride = sizeof(VERTEX_2D);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	pDeviceContext->PSSetSamplers(0, 1, &g_pSamplerState);
	pDeviceContext->PSSetShaderResources(0, 1, &g_pTexture);

	SHADER_GLOBAL cb;
	cb.mProj = XMMatrixTranspose(XMLoadFloat4x4(&g_mProj));
	cb.mView = XMMatrixTranspose(XMLoadFloat4x4(&g_mView));
	cb.mWorld = XMMatrixTranspose(XMLoadFloat4x4(&g_mWorld));
	cb.mTex = XMMatrixTranspose(XMLoadFloat4x4(&g_mTex));
	pDeviceContext->UpdateSubresource(g_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	pDeviceContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
	pDeviceContext->PSSetConstantBuffers(0, 1, &g_pConstantBuffer);

	// �v���~�e�B�u�`����Z�b�g
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �|���S���̕`��
	pDeviceContext->Draw(NUM_VERTEX, 0);
}

//=============================================================================
// ���_�̍쐬
//=============================================================================
HRESULT MakeVertexPolygon(ID3D11Device* pDevice)
{
	// ���_���W�̐ݒ�
	g_vertexWk[0].vtx = XMFLOAT3(-0.5f,  0.5f, 0.0f);
	g_vertexWk[1].vtx = XMFLOAT3( 0.5f,  0.5f, 0.0f);
	g_vertexWk[2].vtx = XMFLOAT3(-0.5f, -0.5f, 0.0f);
	g_vertexWk[3].vtx = XMFLOAT3( 0.5f, -0.5f, 0.0f);

	// �g�U���ˌ��̐ݒ�
	g_vertexWk[0].diffuse = g_colPolygon;
	g_vertexWk[1].diffuse = g_colPolygon;
	g_vertexWk[2].diffuse = g_colPolygon;
	g_vertexWk[3].diffuse = g_colPolygon;

	// �e�N�X�`�����W�̐ݒ�
	g_vertexWk[0].tex = XMFLOAT2(0.0f, 0.0f);
	g_vertexWk[1].tex = XMFLOAT2(1.0f, 0.0f);
	g_vertexWk[2].tex = XMFLOAT2(0.0f, 1.0f);
	g_vertexWk[3].tex = XMFLOAT2(1.0f, 1.0f);

	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(g_vertexWk);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = &g_vertexWk[0];

	HRESULT hr = pDevice->CreateBuffer(&vbd, &initData, &g_pVertexBuffer);

	return hr;
}

//=============================================================================
// ���_���W�̐ݒ�
//=============================================================================
void SetVertexPolygon(void)
{
	if (g_bInvalidate) {
		//���_�o�b�t�@�̒��g�𖄂߂�
		ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
		HRESULT hr = S_OK;

		// ���_�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
		D3D11_MAPPED_SUBRESOURCE msr;
		hr = pDeviceContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
		if (SUCCEEDED(hr)) {
			// �g�U���ˌ��̐ݒ�
			g_vertexWk[0].diffuse = g_colPolygon;
			g_vertexWk[1].diffuse = g_colPolygon;
			g_vertexWk[2].diffuse = g_colPolygon;
			g_vertexWk[3].diffuse = g_colPolygon;
			// ���_�f�[�^���㏑��
			memcpy_s(msr.pData, sizeof(g_vertexWk), g_vertexWk, sizeof(g_vertexWk));
			// ���_�f�[�^���A�����b�N����
			pDeviceContext->Unmap(g_pVertexBuffer, 0);
			// �t���O���N���A
			g_bInvalidate = false;
		}
	}
}

//=============================================================================
// �e�N�X�`���̐ݒ�
//=============================================================================
void SetPolygonTexture(ID3D11ShaderResourceView* pTexture)
{
	g_pTexture = pTexture;
	g_mTex._44 = (g_pTexture) ? 1.0f : 0.0f;
}

//=============================================================================
// �\�����W�̐ݒ�
//=============================================================================
void SetPolygonPos(float fX, float fY)
{
	g_posPolygon.x = fX;
	g_posPolygon.y = fY;
}

//=============================================================================
// �\���T�C�Y�̐ݒ�
//=============================================================================
void SetPolygonSize(float fScaleX, float fScaleY)
{
	g_sizPolygon.x = fScaleX;
	g_sizPolygon.y = fScaleY;
}

//=============================================================================
// �\���p�x�̐ݒ�(�P��:�x)
//=============================================================================
void SetPolygonAngle(float fAngle)
{
	g_rotPolygon.z = fAngle;
}

//=============================================================================
// ����e�N�X�`�����W�̐ݒ� (0.0��fU��1.0, 0.0��fV��1.0)
//=============================================================================
void SetPolygonUV(float fU, float fV)
{
	g_posTexFrame.x = fU;
	g_posTexFrame.y = fV;
}

//=============================================================================
// �e�N�X�`���t���[���T�C�Y�̐ݒ� (0.0��fWidth��1.0, 0.0��fHeight��1.0)
//=============================================================================
void SetPolygonFrameSize(float fWidth, float fHeight)
{
	g_sizTexFrame.x = fWidth;
	g_sizTexFrame.y = fHeight;
}

//=============================================================================
// ���_�J���[�̐ݒ�
//=============================================================================
void SetPolygonColor(float fRed, float fGreen, float fBlue)
{
	if (fRed != g_colPolygon.x || fGreen != g_colPolygon.y || fBlue != g_colPolygon.z) {
		g_colPolygon.x = fRed;
		g_colPolygon.y = fGreen;
		g_colPolygon.z = fBlue;
		g_bInvalidate = true;
	}
}

//=============================================================================
// �s�����x�̐ݒ�
//=============================================================================
void SetPolygonAlpha(float fAlpha)
{
	if (fAlpha != g_colPolygon.w) {
		g_colPolygon.w = fAlpha;
		g_bInvalidate = true;
	}
}
