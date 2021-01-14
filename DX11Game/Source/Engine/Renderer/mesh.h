//=============================================================================
//
// ���b�V�����ʏ��� [mesh.h]
// Author : KOSHI USAMI
//
//=============================================================================
#pragma once

#include "../main.h"
#include "Camera.h"
#include "Light.h"

//*****************************************************************************
// �萔��`
//*****************************************************************************
enum ePrimitiveType {
	PT_UNDEFINED = 0,
	PT_POINT,
	PT_LINE,
	PT_LINESTRIP,
	PT_TRIANGLE,
	PT_TRIANGLESTRIP,

	MAX_PRIMITIVETYPE
};
enum eTranslucentType {
	TT_NOAFFECT = 0,	// �S��
	TT_OPACITYONLY,		// �s�����̂�
	TT_TRANSLUCENTONLY,	// �������̂�

	MAX_TRANSLUCENTTYPE
};

//**************************************
// �\���̒�`
//**************************************
struct MATERIAL {
	XMFLOAT4	Diffuse  =	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);;		// Diffuse color RGBA
	XMFLOAT4	Ambient	 =	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);;		// Ambient color RGB
	XMFLOAT4	Specular =	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);;		// Specular 'shininess'
	XMFLOAT4	Emissive =	XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);;		// Emissive color RGB
	float		Power	 =	50.0f;;			// Sharpness if specular highlight
} ;

struct MESH {
	ID3D11Buffer* pVertexBuffer = nullptr;			// ���_�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^
	ID3D11Buffer* pIndexBuffer = nullptr;				// �C���f�b�N�X�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^

	XMFLOAT4X4 mtxWorld;					// ���[���h�}�g���b�N�X
	//XMFLOAT3 pos;							// �|���S���\���ʒu�̒��S���W
	//XMFLOAT3 rot;							// �|���S���̉�]�p
	int nNumVertex = 0;							// �����_��	
	int nNumIndex = 0;							// ���C���f�b�N�X��

	XMFLOAT4X4 mtxTexture;					// �e�N�X�`�� �}�g���b�N�X
	ID3D11ShaderResourceView* pTexture = nullptr;			// �e�N�X�`��
	ID3D11ShaderResourceView* pNormalTexture = nullptr;		// �@���e�N�X�`��
	ID3D11ShaderResourceView* pAmbientTexture = nullptr;	// ���e�N�X�`��
	bool bLight = true;
	ePrimitiveType primitiveType;			// �v���~�e�B�u�^

	MATERIAL* pMaterial = nullptr;					// �}�e���A��
	float fAlpha;							// ���_�J���[���ŏ��l
} ;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitMesh(void);
void UninitMesh(void);
void UpdateMesh(MESH* pMesh);
void DrawMesh(ID3D11DeviceContext* pDeviceContext, MESH* pMesh, int nTranslucntType = TT_NOAFFECT);
HRESULT MakeMeshVertex(ID3D11Device* pDevice, MESH* pMesh, VERTEX_3D vertexWk[], int indexWk[]);
void ReleaseMesh(MESH* pMesh);
void SetMeshCamera(CCamera* pCamera);
void SetMeshLight(CLight* pLight);
HRESULT UpdateMeshVertex(ID3D11DeviceContext* pDeviceContext, MESH* pMesh, VERTEX_3D vertex[]);
void ClearShadowBuffer();
void DrawMeshShadow(ID3D11DeviceContext* pDeviceContext, MESH* pMesh, int nTranslucntType = TT_NOAFFECT);


extern ID3D11DepthStencilView*		g_pShadowDepthStencilView;	// Z�o�b�t�@
extern ID3D11SamplerState*			g_pShadowSamplerState;		// �e�N�X�`�� �T���v��
extern ID3D11Texture2D*				g_pShadowMap;				// �V���h�E�}�b�v�e�N�X�`��
extern ID3D11ShaderResourceView*	g_pShadowResourceView;		// �V���h�E�}�b�v���\�[�X


class CMesh
{
public:
	CMesh();
	~CMesh();

	static HRESULT InitMesh(void);
	static void UninitMesh(void);

	void UpdateMesh(MESH* pMesh);
	void DrawMesh(ID3D11DeviceContext* pDeviceContext, MESH* pMesh, int nTranslucntType = TT_NOAFFECT);
	HRESULT MakeMeshVertex(ID3D11Device* pDevice, MESH* pMesh, VERTEX_3D vertexWk[], int indexWk[]);
	void ReleaseMesh(MESH* pMesh);
	void SetMeshCamera(CCamera* pCamera);
	void SetMeshLight(CLight* pLight);
	HRESULT UpdateMeshVertex(ID3D11DeviceContext* pDeviceContext, MESH* pMesh, VERTEX_3D vertex[]);
	void ClearShadowBuffer();
	void DrawMeshShadow(ID3D11DeviceContext* pDeviceContext, MESH* pMesh, int nTranslucntType = TT_NOAFFECT);
private:
	ID3D11Buffer* pVertexBuffer;			// ���_�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^
	ID3D11Buffer* pIndexBuffer;				// �C���f�b�N�X�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^

	XMFLOAT4X4 mtxWorld;					// ���[���h�}�g���b�N�X
	XMFLOAT3 pos;							// �|���S���\���ʒu�̒��S���W
	XMFLOAT3 rot;							// �|���S���̉�]�p
	int nNumVertex;							// �����_��	
	int nNumIndex;							// ���C���f�b�N�X��

	XMFLOAT4X4 mtxTexture;					// �e�N�X�`�� �}�g���b�N�X
	ID3D11ShaderResourceView* pTexture;		// �e�N�X�`��
	ID3D11ShaderResourceView* pNormalTexture = nullptr;		// �e�N�X�`��
	bool bBump;

	ePrimitiveType primitiveType;			// �v���~�e�B�u�^

	MATERIAL* pMaterial;					// �}�e���A��
	float fAlpha;
};

