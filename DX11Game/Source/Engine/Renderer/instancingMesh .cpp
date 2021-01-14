//=============================================================================
//
// �C���X�^���V���O���b�V�����ʏ��� [instancingMesh.h]
// Author : KOSHI USAMI
//
//=============================================================================
#include "instancingMesh.h"
#include "../System/Shader.h"
#include "../System/Texture.h"

#include "../System/polygon.h"
#include "../System/input.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define M_DIFFUSE		XMFLOAT4(1.0f,1.0f,1.0f,1.0f)
#define M_SPECULAR		XMFLOAT4(0.1f,0.1f,0.1f,1.0f)
#define M_AMBIENT		XMFLOAT4(1.0f,1.0f,1.0f,1.0f)
#define M_EMISSIVE		XMFLOAT4(0.0f,0.0f,0.0f,1.0f)
#define MAX_INSTANCE (1024)


//*****************************************************************************
// �\���̒�`
//*****************************************************************************
// �C���X�^���V���O�f�[�^
struct InstancingMatrix
{
	XMMATRIX mWorld;
	//XMMATRIX mTexture;
};

// �V�F�[�_�ɓn���l
struct SHADER_GLOBAL {
	XMMATRIX	mVP;		// ���[���h�~�r���[�~�ˉe�s��(�]�u�s��)
	XMMATRIX	mLightVP;		// ���[���h�~�r���[�~�ˉe�s��(�]�u�s��)
	XMMATRIX	mTexture;
	XMVECTOR	vFog;
};
struct INSTANCING_GLOBAL
{
	InstancingMatrix aInstancing[MAX_INSTANCE];
};

// �C���X�^���V���O�}�e���A��
//struct InstancingMaterial
//{
//	// �}�e���A��
//	XMVECTOR	vAmbient;	// �A���r�G���g�F(+�e�N�X�`���L��)
//	XMVECTOR	vDiffuse;	// �f�B�t���[�Y�F
//	XMVECTOR	vSpecular;	// �X�y�L�����F(+�X�y�L�������x)
//	XMVECTOR	vEmissive;	// �G�~�b�V�u�F
//
//	int		bBump = false;
//};

struct SHADER_GLOBAL2 {
	XMVECTOR	vEye;		// ���_���W
	// ����
	XMVECTOR	vLightDir;	// ��������
	XMVECTOR	vLa;		// �����F(�A���r�G���g)
	XMVECTOR	vLd;		// �����F(�f�B�t���[�Y)
	XMVECTOR	vLs;		// �����F(�X�y�L����)

		// �}�e���A��
	XMVECTOR	vAmbient;	// �A���r�G���g�F(+�e�N�X�`���L��)
	XMVECTOR	vDiffuse;	// �f�B�t���[�Y�F
	XMVECTOR	vSpecular;	// �X�y�L�����F(+�X�y�L�������x)
	XMVECTOR	vEmissive;	// �G�~�b�V�u�F

	int		bBump = false;
};
//struct INSTANCING_GLOBAL2
//{
//	InstancingMaterial aInstancing[MAX_INSTANCE];
//};


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer*				g_pConstantBuffer[4];	// �萔�o�b�t�@
static ID3D11SamplerState*			g_pSamplerState;		// �e�N�X�`�� �T���v��
static ID3D11VertexShader*			g_pVertexShader;		// ���_�V�F�[�_
static ID3D11InputLayout*			g_pInputLayout;			// ���_�t�H�[�}�b�g
static ID3D11PixelShader*			g_pPixelShader;			// �s�N�Z���V�F�[�_

static MATERIAL						g_material;				// �}�e���A��
static CCamera*						g_pCamera;				// �J����
static CLight*						g_pLight;				// ���C�g

static const XMMATRIX SHADOW_BIAS = XMMATRIX(
	0.5f, 0.0f, 0.0f, 0.0f,
	0.0f, -0.5f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.5f, 0.5f, 0.0f, 1.0f);

static INSTANCING_GLOBAL icb;
//static INSTANCING_GLOBAL2 icb2;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitInstancingMesh(void)
{
	ID3D11Device* pDevice = GetDevice();
	HRESULT hr;

	// �V�F�[�_������
	static const D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	hr = LoadShader("InstancingVertex", "InstancingPixel", &g_pVertexShader, &g_pInputLayout, &g_pPixelShader, layout, _countof(layout));
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
	hr = pDevice->CreateBuffer(&bd, nullptr, &g_pConstantBuffer[0]);
	if (FAILED(hr)) return hr;
	bd.ByteWidth = sizeof(SHADER_GLOBAL2);
	hr = pDevice->CreateBuffer(&bd, nullptr, &g_pConstantBuffer[1]);
	if (FAILED(hr)) return hr;
	bd.ByteWidth = sizeof(INSTANCING_GLOBAL);
	hr = pDevice->CreateBuffer(&bd, nullptr, &g_pConstantBuffer[2]);
	if (FAILED(hr)) return hr;
	/*bd.ByteWidth = sizeof(INSTANCING_GLOBAL2);
	hr = pDevice->CreateBuffer(&bd, nullptr, &g_pConstantBuffer[3]);
	if (FAILED(hr)) return hr;*/

	// �e�N�X�`�� �T���v������
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	hr = pDevice->CreateSamplerState(&sd, &g_pSamplerState);
	if (FAILED(hr)) {
		return hr;
	}

	// �}�e���A���̏����ݒ�
	g_material.Diffuse = M_DIFFUSE;
	g_material.Ambient = M_AMBIENT;
	g_material.Specular = M_SPECULAR;
	g_material.Power = 0.0f;
	g_material.Emissive = M_EMISSIVE;

	return hr;
}

//=============================================================================
// �I������
//=============================================================================
void UninitInstancingMesh(void)
{
	// �e�N�X�`�� �T���v���̊J��
	SAFE_RELEASE(g_pSamplerState);
	// �萔�o�b�t�@�̉��
	for (int i = 0; i < _countof(g_pConstantBuffer); ++i) {
		SAFE_RELEASE(g_pConstantBuffer[i]);
	}
	// �s�N�Z���V�F�[�_���
	SAFE_RELEASE(g_pPixelShader);
	// ���_�t�H�[�}�b�g���
	SAFE_RELEASE(g_pInputLayout);
	// ���_�V�F�[�_���
	SAFE_RELEASE(g_pVertexShader);
}

////=============================================================================
//// �X�V����
////=============================================================================
//void UpdateInstancingMesh(InstancingMesh* pInstancingMesh)
//{
//	XMMATRIX mtxWorld, mtxRot, mtxTranslate;
//
//	if (!pInstancingMesh) return;
//
//	// ���[���h�}�g���b�N�X�̏�����
//	mtxWorld = XMMatrixIdentity();
//
//	// ��]�𔽉f
//	mtxRot = XMMatrixRotationRollPitchYaw(pInstancingMesh->rot.x, pInstancingMesh->rot.y, pInstancingMesh->rot.z);
//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);
//
//	// �ړ��𔽉f
//	mtxTranslate = XMMatrixTranslation(pInstancingMesh->pos.x, pInstancingMesh->pos.y, pInstancingMesh->pos.z);
//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);
//
//	// ���[���h�}�g���b�N�X�̐ݒ�
//	XMStoreFloat4x4(&pInstancingMesh->mtxWorld, mtxWorld);
//}


void DrawInstancingMeshShadow(ID3D11DeviceContext* pDeviceContext, InstancingMesh* pInstancingMesh,
	std::vector<InstancingMeshData*>& InstancingList, int nTranslucntType)
{
	{
		// �V���h�E�}�b�v
		// �e�^�[�Q�b�g�r���[�������_�[�^�[�Q�b�g�ɐݒ�
		pDeviceContext->OMSetRenderTargets(0, nullptr, g_pShadowDepthStencilView);

		// �V�F�[�_�ݒ�
		pDeviceContext->VSSetShader(g_pVertexShader, nullptr, 0);
		pDeviceContext->PSSetShader(nullptr, nullptr, 0);
		pDeviceContext->IASetInputLayout(g_pInputLayout);

		// ���_�o�b�t�@���Z�b�g
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		pDeviceContext->IASetVertexBuffers(0, 1, &pInstancingMesh->pVertexBuffer, &stride, &offset);
		// �C���f�b�N�X�o�b�t�@���Z�b�g
		pDeviceContext->IASetIndexBuffer(pInstancingMesh->pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		//===== �T���v���[�A�e�N�X�`�� =====
		ID3D11SamplerState* pState[2] = { g_pSamplerState, g_pShadowSamplerState };
		ID3D11ShaderResourceView* pResource[3] = { pInstancingMesh->pTexture, g_pShadowResourceView, pInstancingMesh->pNormalTexture };
		pDeviceContext->PSSetSamplers(0, 2, pState);
		pDeviceContext->PSSetShaderResources(0, 3, pResource);
		// �v���~�e�B�u
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		// ���ʃV�F�[�_�[�f�[�^
		SHADER_GLOBAL cb;
		cb.mVP = XMMatrixTranspose(XMLoadFloat4x4(&g_pLight->GetViewMatrix()) * XMLoadFloat4x4(&g_pCamera->GetProjMatrix()));
		cb.mLightVP = XMMatrixTranspose(XMLoadFloat4x4(&g_pLight->GetViewMatrix()) * XMLoadFloat4x4(&g_pLight->GetProjMatrix()));
		cb.mTexture = XMMatrixTranspose(XMLoadFloat4x4(pInstancingMesh->mtxTexture));
		pDeviceContext->UpdateSubresource(g_pConstantBuffer[0], 0, nullptr, &cb, 0, 0);
		pDeviceContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer[0]);

		//================== �C���X�^���V���O�f�[�^ ==================
		// �J�����s��擾
		XMFLOAT4X4& mtxView = CCamera::GetMainCamera()->GetViewMatrix();

		int count = (int)InstancingList.size();
		int n = (count - 1) / MAX_INSTANCE + 1;
		for (int i = 0; i < n; i++) 
		{
			int m = (i < n - 1 ? MAX_INSTANCE : count - i * MAX_INSTANCE);
			for (int j = 0; j < m; ++j)
			{
				InstancingMeshData* pData = InstancingList[i * MAX_INSTANCE + j];

				if (pInstancingMesh->bBillboard)
				{
					// �}�g���b�N�X�쐬
					XMMATRIX mtxWorld, mtxScale, mtxTranslate;
					// ���[���h�}�g���b�N�X�̏�����
					mtxWorld = XMMatrixIdentity();
					XMFLOAT4X4* m_mtxWorld = pData->mtxWorld;
					XMStoreFloat4x4(m_mtxWorld, mtxWorld);

					m_mtxWorld->_11 = mtxView._11;
					m_mtxWorld->_12 = mtxView._21;
					m_mtxWorld->_13 = mtxView._31;
					m_mtxWorld->_21 = mtxView._12;
					m_mtxWorld->_22 = mtxView._22;
					m_mtxWorld->_23 = mtxView._32;
					m_mtxWorld->_31 = mtxView._13;
					m_mtxWorld->_32 = mtxView._23;
					m_mtxWorld->_33 = mtxView._33;

					mtxWorld = XMLoadFloat4x4(m_mtxWorld);

					// �X�P�[���𔽉f
					mtxScale = XMMatrixScaling(pData->pScale->x, pData->pScale->y, 1.0f);
					mtxWorld = XMMatrixMultiply(mtxScale, mtxWorld);

					// �ړ��𔽉f
					mtxTranslate = XMMatrixTranslation(pData->pPos->x, pData->pPos->y, pData->pPos->z);
					mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

					// ���[���h�}�g���b�N�X�̐ݒ�
					XMStoreFloat4x4(m_mtxWorld, mtxWorld);
					icb.aInstancing[j].mWorld = XMMatrixTranspose(XMLoadFloat4x4(m_mtxWorld));
				}
				else
				{
					icb.aInstancing[j].mWorld = XMMatrixTranspose(XMLoadFloat4x4(pData->mtxWorld));
					//icb.aInstancing[j].mTexture = XMLoadFloat4x4(pData->mtxTexture);
				}
			}
			pDeviceContext->UpdateSubresource(g_pConstantBuffer[2], 0, nullptr, &icb, 0, 0);
			pDeviceContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer[2]);
			// �|���S���̕`��
			pDeviceContext->DrawIndexedInstanced(pInstancingMesh->nNumIndex, m, 0, 0, 0);
		}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawInstancingMesh(ID3D11DeviceContext* pDeviceContext, InstancingMesh* pInstancingMesh,
	std::vector<InstancingMeshData*>& InstancingList, int nTranslucntType)
{
	//=============================================================================================
	// �e�^�[�Q�b�g�r���[�������_�[�^�[�Q�b�g�ɐݒ�
	SetRenderTarget();

	// �V�F�[�_�ݒ�
	pDeviceContext->VSSetShader(g_pVertexShader, nullptr, 0);
	pDeviceContext->PSSetShader(g_pPixelShader, nullptr, 0);
	pDeviceContext->IASetInputLayout(g_pInputLayout);

	// ���_�o�b�t�@���Z�b�g
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &pInstancingMesh->pVertexBuffer, &stride, &offset);
	// �C���f�b�N�X�o�b�t�@���Z�b�g
	pDeviceContext->IASetIndexBuffer(pInstancingMesh->pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);


	//===== �T���v���[�A�e�N�X�`�� =====
	ID3D11SamplerState* pState[2] = { g_pSamplerState, g_pShadowSamplerState };
	ID3D11ShaderResourceView* pResource[3] = { pInstancingMesh->pTexture, g_pShadowResourceView, pInstancingMesh->pNormalTexture };

	pDeviceContext->PSSetSamplers(0, 2, pState);
	pDeviceContext->PSSetShaderResources(0, 3, pResource);

	SHADER_GLOBAL cb;
	cb.mVP = XMMatrixTranspose(XMLoadFloat4x4(&g_pCamera->GetViewMatrix()) * XMLoadFloat4x4(&g_pCamera->GetProjMatrix()));
	cb.mLightVP = XMMatrixTranspose(XMLoadFloat4x4(&g_pLight->GetViewMatrix()) * XMLoadFloat4x4(&g_pCamera->GetProjMatrix()) * SHADOW_BIAS);
	cb.mTexture = XMMatrixTranspose(XMLoadFloat4x4(pInstancingMesh->mtxTexture));
	XMFLOAT2 fog = { FOG_FAR_Z / (FOG_FAR_Z - FOG_NEAR_Z), -1 / (FOG_FAR_Z - FOG_NEAR_Z) };
	cb.vFog = XMLoadFloat2(&fog);
	pDeviceContext->UpdateSubresource(g_pConstantBuffer[0], 0, nullptr, &cb, 0, 0);
	pDeviceContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer[0]);

	SHADER_GLOBAL2 cb2;
	cb2.vEye = XMLoadFloat3(&g_pCamera->GetPos());
	cb2.vLightDir = XMLoadFloat3(&g_pLight->GetDir());
	cb2.vLa = XMLoadFloat4(&g_pLight->GetAmbient());
	cb2.vLd = XMLoadFloat4(&g_pLight->GetDiffuse());
	cb2.vLs = XMLoadFloat4(&g_pLight->GetSpecular());

	cb2.vDiffuse = XMLoadFloat4(&pInstancingMesh->material.Diffuse);
	cb2.vAmbient = XMVectorSet(pInstancingMesh->material.Ambient.x, pInstancingMesh->material.Ambient.y, pInstancingMesh->material.Ambient.z, (pInstancingMesh->pTexture != nullptr) ? 1.f : 0.f);
	cb2.vSpecular = XMVectorSet(pInstancingMesh->material.Specular.x, pInstancingMesh->material.Specular.y, pInstancingMesh->material.Specular.z, pInstancingMesh->material.Power);
	cb2.vEmissive = XMLoadFloat4(&pInstancingMesh->material.Emissive);

	cb2.bBump = pInstancingMesh->bBump;

	pDeviceContext->UpdateSubresource(g_pConstantBuffer[1], 0, nullptr, &cb2, 0, 0);
	pDeviceContext->PSSetConstantBuffers(1, 1, &g_pConstantBuffer[1]);

	// �v���~�e�B�u�`����Z�b�g
	static const D3D11_PRIMITIVE_TOPOLOGY pt[] = {
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,	// 0�Ȃ�O�p�`�X�g���b�v
		D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
		D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
	};
	pDeviceContext->IASetPrimitiveTopology(pt[pInstancingMesh->primitiveType]);

	// �u�����h�X�e�[�g
	SetBlendState(pInstancingMesh->blendState);

	//================== �C���X�^���V���O�f�[�^ ==================
			// �J�����s��擾
	XMFLOAT4X4& mtxView = CCamera::GetMainCamera()->GetViewMatrix();

	int count = (int)InstancingList.size();
	int n = (count - 1) / MAX_INSTANCE + 1;
	for (int i = 0; i < n; i++)
	{
		int m = (i < n - 1 ? MAX_INSTANCE : count - i * MAX_INSTANCE);
		for (int j = 0; j < m; ++j)
		{
			InstancingMeshData* pData = InstancingList[i * MAX_INSTANCE + j];

			if (pInstancingMesh->bBillboard)
			{
				// �}�g���b�N�X�쐬
				XMMATRIX mtxWorld, mtxScale, mtxTranslate;
				// ���[���h�}�g���b�N�X�̏�����
				mtxWorld = XMMatrixIdentity();
				XMFLOAT4X4* m_mtxWorld = pData->mtxWorld;
				XMStoreFloat4x4(m_mtxWorld, mtxWorld);

				m_mtxWorld->_11 = mtxView._11;
				m_mtxWorld->_12 = mtxView._21;
				m_mtxWorld->_13 = mtxView._31;
				m_mtxWorld->_21 = mtxView._12;
				m_mtxWorld->_22 = mtxView._22;
				m_mtxWorld->_23 = mtxView._32;
				m_mtxWorld->_31 = mtxView._13;
				m_mtxWorld->_32 = mtxView._23;
				m_mtxWorld->_33 = mtxView._33;

				mtxWorld = XMLoadFloat4x4(m_mtxWorld);

				// �X�P�[���𔽉f
				mtxScale = XMMatrixScaling(pData->pScale->x, pData->pScale->y, 1.0f);
				mtxWorld = XMMatrixMultiply(mtxScale, mtxWorld);

				// �ړ��𔽉f
				mtxTranslate = XMMatrixTranslation(pData->pPos->x, pData->pPos->y, pData->pPos->z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

				// ���[���h�}�g���b�N�X�̐ݒ�
				XMStoreFloat4x4(m_mtxWorld, mtxWorld);
				icb.aInstancing[j].mWorld = XMMatrixTranspose(XMLoadFloat4x4(m_mtxWorld));
			}
			else
			{
				icb.aInstancing[j].mWorld = XMMatrixTranspose(XMLoadFloat4x4(pData->mtxWorld));
				//icb.aInstancing[j].mTexture = XMLoadFloat4x4(pData->mtxTexture);
			}
		}
		pDeviceContext->UpdateSubresource(g_pConstantBuffer[2], 0, nullptr, &icb, 0, 0);
		pDeviceContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer[2]);
		/*pDeviceContext->UpdateSubresource(g_pConstantBuffer[3], 0, nullptr, &icb2, 0, 0);
		pDeviceContext->PSSetConstantBuffers(3, 1, &g_pConstantBuffer[3]);*/

		// �|���S���̕`��
		pDeviceContext->DrawIndexedInstanced(pInstancingMesh->nNumIndex, m, 0, 0, 0);
	}

	// �u�����h�X�e�[�g
	SetBlendState(BS_NONE);
}

//=============================================================================
// ���_�̍쐬
//=============================================================================
HRESULT MakeInstancingMeshVertex(ID3D11Device* pDevice, InstancingMesh* pInstancingMesh, VERTEX_3D vertexWk[], int indexWk[])
{
	pInstancingMesh->fAlpha = 1.0f;
	for (int i = 0; i < pInstancingMesh->nNumVertex; ++i) {
		if (pInstancingMesh->fAlpha > vertexWk[i].diffuse.w) {
			pInstancingMesh->fAlpha = vertexWk[i].diffuse.w;
		}
	}

	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(VERTEX_3D) * pInstancingMesh->nNumVertex;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = vertexWk;
	HRESULT hr = pDevice->CreateBuffer(&vbd, &initData, &pInstancingMesh->pVertexBuffer);
	if (FAILED(hr)) {
		return hr;
	}

	CD3D11_BUFFER_DESC ibd(pInstancingMesh->nNumIndex * sizeof(int), D3D11_BIND_INDEX_BUFFER);
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = indexWk;
	hr = pDevice->CreateBuffer(&ibd, &initData, &pInstancingMesh->pIndexBuffer);

	return hr;
}

//=============================================================================
// ���b�V�����
//=============================================================================
void ReleaseInstancingMesh(InstancingMesh* pInstancingMesh)
{
	if (!pInstancingMesh) return;
	// �e�N�X�`�����
	SAFE_RELEASE(pInstancingMesh->pTexture);
	// ���_�o�b�t�@���
	SAFE_RELEASE(pInstancingMesh->pVertexBuffer);
	// �C���f�b�N�X �o�b�t�@���
	SAFE_RELEASE(pInstancingMesh->pIndexBuffer);
}

//=============================================================================
// �J�����̐ݒ�
//=============================================================================
void SetInstancingMeshCamera(CCamera* pCamera)
{
	g_pCamera = pCamera;
}

//=============================================================================
// ���C�g�̐ݒ�
//=============================================================================
void SetInstancingMeshLight(CLight* pLight)
{
	g_pLight = pLight;
}

//=============================================================================
// ���_�o�b�t�@�X�V
//=============================================================================
HRESULT UpdateInstancingMeshVertex(ID3D11DeviceContext* pDeviceContext, InstancingMesh* pInstancingMesh, VERTEX_3D vertex[])
{
	// ���_�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
	D3D11_MAPPED_SUBRESOURCE msr;
	HRESULT hr = pDeviceContext->Map(pInstancingMesh->pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	if (SUCCEEDED(hr)) {
		// ���_�f�[�^���㏑��
		rsize_t len = sizeof(VERTEX_3D) * pInstancingMesh->nNumVertex;
		memcpy_s(msr.pData, len, vertex, len);
		// ���_�f�[�^���A�����b�N����
		pDeviceContext->Unmap(pInstancingMesh->pVertexBuffer, 0);
	}
	return hr;
}
