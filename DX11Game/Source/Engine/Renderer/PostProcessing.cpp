//======================================================================
//											PostProcessing.cpp
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


//===== �C���N���[�h�� =====
#include "PostProcessing.h"
#include "../System/Shader.h"
#include "../System/polygon.h"
#include "../System/Texture.h"

//===== �}�N����` =====


//===== �\���� =====
struct GaussBlurParam
{
	int		 nSampleCount;
	XMFLOAT4 Offset[16];
};


// ���_�V�F�[�_�[
struct VERTEX_SHADER_GLOBAL {
	XMMATRIX	mWorld;		// ���[���h�ϊ��s��(�]�u�s��)
	XMMATRIX	mView;		// �r���[�ϊ��s��(�]�u�s��)
	XMMATRIX	mProj;		// �ˉe�ϊ��s��(�]�u�s��)
	XMMATRIX	mTex;		// �e�N�X�`���ϊ��s��(�]�u�s��)
};
// �s�N�Z���V�F�[�_�[
struct PIXEL_SHADER_GLOBAL
{
	XMMATRIX	mTex;			// �e�N�X�`���ϊ��s��(�]�u�s��)
	XMVECTOR	vColor;			// �J���[
	int			nSampleCount;	// �T���v����(���[�v)
	XMVECTOR	vOffset[16];	// �K�E�X�u���[�̃I�t�Z�b�g
};


//===== �v���g�^�C�v�錾 =====
//      �K�E�X�̏d�݂��v�Z���܂�.
float GaussianDistribution(const XMFLOAT2& pos, float rho);
//      �u���[�p�����[�^���v�Z���܂�.
GaussBlurParam CalcBlurParam(int width, int height, XMFLOAT2 dir, float deviation);


//===== �O���[�o���ϐ� =====
static GaussBlurParam g_gaussParam;
PostProcessing* PostProcessing::m_pMain = nullptr;



//===================================
//
//	�R���X�g���N�^
//
//===================================
PostProcessing::PostProcessing()
{
	m_pVertexShader = NULL;		// ���_�V�F�[�_
	m_pPixelShader = NULL;			// �s�N�Z���V�F�[�_
	m_pInputLayout = NULL;			// ���_�t�H�[�}�b�g

	m_pVertexConstantBuffer = NULL;		// ���_�V�F�[�_�[�萔�o�b�t�@
	m_pPixelConstantBuffer = NULL;			// �s�N�Z���V�F�[�_�[�萔�o�b�t�@
	m_pSamplerState = NULL;				// �e�N�X�`�� �T���v��
	m_pVertexBuffer = NULL;				// ���_�o�b�t�@

	m_pRenderTargetView = NULL;	// �����_�[�^�[�Q�b�g
	m_pRenderTexture = NULL;		// �����_�[�e�N�X�`��
	m_pTexureResourceView = NULL;	// �e�N�X�`�����\�[�X

	// �ϊ��s�񏉊���
	XMStoreFloat4x4(&m_mWorld, XMMatrixIdentity());
	XMStoreFloat4x4(&m_mView, XMMatrixLookAtLH(XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));
	XMStoreFloat4x4(&m_mProj,
		XMMatrixOrthographicLH(SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f, 100.0f));
	XMStoreFloat4x4(&m_mTex, XMMatrixIdentity());
	m_mTex._44 = 0.0f;

	m_pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_siz = XMFLOAT3(SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f);

	m_col = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	m_posTex = XMFLOAT2(0.0f, 0.0f);
	m_sizTex = XMFLOAT2(1.0f, 1.0f);


	// �K�E�X�u���[�̃p�����[�^
	g_gaussParam = CalcBlurParam(1.3f, 1.3f, XMFLOAT2{ 1,1 }, 1.0f);

	m_pMain = this;
}

//===================================
//
//	�f�X�g���N�^
//
//===================================
PostProcessing::~PostProcessing()
{
}

//===================================
//
//	����������
//
//===================================
HRESULT PostProcessing::Init(ID3D11Device* pDevice)
{
	HRESULT hr;

	// �V�F�[�_������
	static const D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	hr = LoadShader("PostVertex2D", "PostPixel2D", &m_pVertexShader, &m_pInputLayout, &m_pPixelShader, layout, _countof(layout));
	if (FAILED(hr)) {
		return hr;
	}

	// �萔�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX_SHADER_GLOBAL);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = pDevice->CreateBuffer(&bd, nullptr, &m_pVertexConstantBuffer);
	if (FAILED(hr)) return hr;
	bd.ByteWidth = sizeof(PIXEL_SHADER_GLOBAL);
	hr = pDevice->CreateBuffer(&bd, nullptr, &m_pPixelConstantBuffer);
	if (FAILED(hr)) return hr;

	// �e�N�X�`�� �T���v������
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	hr = pDevice->CreateSamplerState(&sd, &m_pSamplerState);
	if (FAILED(hr)) {
		return hr;
	}

	// ���_�̍쐬
	hr = MakeVertex(pDevice);
	if (FAILED(hr)) {
		return hr;
	}

	// �����_�[�e�N�X�`��
	D3D11_TEXTURE2D_DESC renderTexture;
	ZeroMemory(&renderTexture, sizeof(D3D11_TEXTURE2D_DESC));
	//renderTexture.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	renderTexture.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	renderTexture.MipLevels = 1;
	renderTexture.ArraySize = 1;
	renderTexture.SampleDesc = GetMSAA();
	renderTexture.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	renderTexture.Height = static_cast<UINT>(SCREEN_HEIGHT);
	renderTexture.Width = static_cast<UINT>(SCREEN_WIDTH);

	hr = pDevice->CreateTexture2D(
		&renderTexture,
		nullptr,
		&m_pRenderTexture
	);
	if (FAILED(hr)) {
		return hr;
	}

	// �����_�[�^�[�Q�b�g�r���[
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetView;
	ZeroMemory(&renderTargetView, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	renderTargetView.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//renderTargetView.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetView.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;

	hr = pDevice->CreateRenderTargetView(
		m_pRenderTexture,
		&renderTargetView,
		&m_pRenderTargetView
	);
	if (FAILED(hr)) {
		return hr;
	}

	// �����_�[�e�N�X�`���̃V�F�[�_�[���\�[�X
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	shaderResourceViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;

	hr = pDevice->CreateShaderResourceView(
		m_pRenderTexture,
		NULL,
		&m_pTexureResourceView
	);
	if (FAILED(hr)) {
		return hr;
	}


	// �[�x�X�e���V���e�N�X�`��
	D3D11_TEXTURE2D_DESC shadowMapDesc;
	ZeroMemory(&shadowMapDesc, sizeof(D3D11_TEXTURE2D_DESC));
	shadowMapDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	shadowMapDesc.MipLevels = 1;
	shadowMapDesc.ArraySize = 1;
	shadowMapDesc.SampleDesc = GetMSAA();
	shadowMapDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	shadowMapDesc.Height = static_cast<UINT>(SCREEN_HEIGHT);
	shadowMapDesc.Width = static_cast<UINT>(SCREEN_WIDTH);

	hr = pDevice->CreateTexture2D(
		&shadowMapDesc,
		nullptr,
		&m_pDepthStencTexure
	);
	if (FAILED(hr)) {
		return hr;
	}

	// �[�x�X�e���V���r���[
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	hr = pDevice->CreateDepthStencilView(
		m_pDepthStencTexure,
		&depthStencilViewDesc,
		&m_pDepthStencilView
	);
	if (FAILED(hr)) {
		return hr;
	}

	// �[�x�X�e���V���V�F�[�_�[���\�[�X
	D3D11_SHADER_RESOURCE_VIEW_DESC DSshaderResourceViewDesc;
	ZeroMemory(&DSshaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	//DSshaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	DSshaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	DSshaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	DSshaderResourceViewDesc.Texture2D.MipLevels = 1;

	hr = pDevice->CreateShaderResourceView(
		m_pDepthStencTexure,
		&DSshaderResourceViewDesc,
		&m_pDepthStencResourceView
	);
	if (FAILED(hr)) {
		return hr;
	}

	// �[�x�X�e���V���T���v�����[
	D3D11_SAMPLER_DESC comparisonSamplerDesc;
	ZeroMemory(&comparisonSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	comparisonSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	comparisonSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	comparisonSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	comparisonSamplerDesc.BorderColor[0] = 1.0f;
	comparisonSamplerDesc.BorderColor[1] = 1.0f;
	comparisonSamplerDesc.BorderColor[2] = 1.0f;
	comparisonSamplerDesc.BorderColor[3] = 1.0f;
	comparisonSamplerDesc.MinLOD = 0.f;
	comparisonSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	comparisonSamplerDesc.MipLODBias = 0.f;
	comparisonSamplerDesc.MaxAnisotropy = 0;
	comparisonSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	comparisonSamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;

	hr = pDevice->CreateSamplerState(
		&comparisonSamplerDesc,
		&m_pDepthSamplerState
	);
	if (FAILED(hr)) {
		return hr;
	}


	return hr;
}

//===================================
//
//	�I������
//
//===================================
void PostProcessing::Uninit()
{


}

//===================================
//
//	�`�揈��
//
//===================================
void PostProcessing::Draw(ID3D11DeviceContext* pDeviceContext)
{
	// �g�k
	XMMATRIX mWorld = XMMatrixScaling(m_siz.x, m_siz.y, m_siz.z);
	// ��]
	mWorld *= XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_rot.x),
		XMConvertToRadians(m_rot.y), XMConvertToRadians(m_rot.z));
	// �ړ�
	mWorld *= XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
	// ���[���h �}�g���b�N�X�ɐݒ�
	XMStoreFloat4x4(&m_mWorld, mWorld);

	if (m_pRenderTexture) {
		// �g�k
		mWorld = XMMatrixScaling(m_sizTex.x, m_sizTex.y, 1.0f);
		// �ړ�
		mWorld *= XMMatrixTranslation(m_posTex.x, m_posTex.y, 0.0f);
		// �e�N�X�`�� �}�g���b�N�X�ɐݒ�
		XMStoreFloat4x4(&m_mTex, mWorld);
	}
	else {
		// �e�N�X�`������
		m_mTex._44 = 0.0f;
	}

	pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
	pDeviceContext->IASetInputLayout(m_pInputLayout);

	UINT stride = sizeof(VERTEX_2D);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	//===== �T���v���[�A�e�N�X�`�� =====
	ID3D11SamplerState* pState[2] = { m_pSamplerState, m_pDepthSamplerState };
	ID3D11ShaderResourceView* pResource[2] = { m_pTexureResourceView, m_pDepthStencResourceView};

	pDeviceContext->PSSetSamplers(0, 2, pState);
	pDeviceContext->PSSetShaderResources(0, 2, pResource);

	//pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerState);
	//pDeviceContext->PSSetShaderResources(0, 1, &m_pTexureResourceView);


	VERTEX_SHADER_GLOBAL vcb;
	vcb.mProj = XMMatrixTranspose(XMLoadFloat4x4(&m_mProj));
	vcb.mView = XMMatrixTranspose(XMLoadFloat4x4(&m_mView));
	vcb.mWorld = XMMatrixTranspose(XMLoadFloat4x4(&m_mWorld));
	vcb.mTex = XMMatrixTranspose(XMLoadFloat4x4(&m_mTex));
	pDeviceContext->UpdateSubresource(m_pVertexConstantBuffer, 0, nullptr, &vcb, 0, 0);
	pDeviceContext->VSSetConstantBuffers(0, 1, &m_pVertexConstantBuffer);

	PIXEL_SHADER_GLOBAL pcb;
	pcb.mTex = XMMatrixTranspose(XMLoadFloat4x4(&m_mTex));
	pcb.vColor = XMLoadFloat4(&m_col);
	// �K�E�X�u���[
	pcb.nSampleCount = g_gaussParam.nSampleCount;
	for (int i = 0; i < 16; i++)
	{
		pcb.vOffset[i] = XMLoadFloat4(&g_gaussParam.Offset[i]);
	}
	pDeviceContext->UpdateSubresource(m_pPixelConstantBuffer, 0, nullptr, &pcb, 0, 0);
	pDeviceContext->PSSetConstantBuffers(1, 1, &m_pPixelConstantBuffer);

	// �v���~�e�B�u�`����Z�b�g
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �|���S���̕`��
	pDeviceContext->Draw(NUM_VERTEX, 0);
}

HRESULT PostProcessing::MakeVertex(ID3D11Device* pDevice)
{
	// ���_���W�̐ݒ�
	m_vertexWk[0].vtx = XMFLOAT3(-0.5f, 0.5f, 0.0f);
	m_vertexWk[1].vtx = XMFLOAT3(0.5f, 0.5f, 0.0f);
	m_vertexWk[2].vtx = XMFLOAT3(-0.5f, -0.5f, 0.0f);
	m_vertexWk[3].vtx = XMFLOAT3(0.5f, -0.5f, 0.0f);
	
	// �g�U���ˌ��̐ݒ�
	m_vertexWk[0].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_vertexWk[1].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_vertexWk[2].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_vertexWk[3].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// �e�N�X�`�����W�̐ݒ�
	m_vertexWk[0].tex = XMFLOAT2(0.0f, 0.0f);
	m_vertexWk[1].tex = XMFLOAT2(1.0f, 0.0f);
	m_vertexWk[2].tex = XMFLOAT2(0.0f, 1.0f);
	m_vertexWk[3].tex = XMFLOAT2(1.0f, 1.0f);

	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(m_vertexWk);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = &m_vertexWk[0];

	HRESULT hr = pDevice->CreateBuffer(&vbd, &initData, &m_pVertexBuffer);

	return hr;
}



//-------------------------------------------------------------------------------------------------
//      �K�E�X�̏d�݂��v�Z���܂�.
//-------------------------------------------------------------------------------------------------
float GaussianDistribution(const XMFLOAT2& pos, float rho)
{
	return exp(-(pos.x * pos.x + pos.y * pos.y) / (2.0f * rho * rho));
}

//-------------------------------------------------------------------------------------------------
//      �u���[�p�����[�^���v�Z���܂�.
//-------------------------------------------------------------------------------------------------
GaussBlurParam CalcBlurParam(int width, int height, XMFLOAT2 dir, float deviation)
{
	GaussBlurParam result;
	result.nSampleCount = 15;
	auto tu = 1.0f / float(width);
	auto tv = 1.0f / float(height);

	result.Offset[0].z = GaussianDistribution(XMFLOAT2(0.0f, 0.0f), deviation);
	auto total_weight = result.Offset[0].z;

	result.Offset[0].x = 0.0f;
	result.Offset[0].y = 0.0f;

	for (auto i = 1; i < 8; ++i)
	{
		result.Offset[i].x = dir.x * i * tu;
		result.Offset[i].y = dir.y * i * tv;
		result.Offset[i].z = GaussianDistribution(XMFLOAT2(dir.x * float(i), dir.y * float(i)), deviation);
		total_weight += result.Offset[i].z * 2.0f;
	}

	for (auto i = 0; i < 8; ++i)
	{
		result.Offset[i].z /= total_weight;
	}
	for (auto i = 8; i < 15; ++i)
	{
		result.Offset[i].x = -result.Offset[i - 7].x;
		result.Offset[i].y = -result.Offset[i - 7].y;
		result.Offset[i].z = result.Offset[i - 7].z;
	}

	return result;
}