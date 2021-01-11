//=============================================================================
//
// �C���X�^���V���O���b�V�����ʏ��� [instancingMesh.h]
// Author : KOSHI USAMI
//
//=============================================================================
#pragma once

#include "../main.h"
#include "Camera.h"
#include "Light.h"
#include "mesh.h"
#include <vector>


struct InstancingMesh {
	ID3D11Buffer* pVertexBuffer = nullptr;			// ���_�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^
	ID3D11Buffer* pIndexBuffer = nullptr;				// �C���f�b�N�X�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^
	int nNumVertex = 0;							// �����_��	
	int nNumIndex = 0;							// ���C���f�b�N�X��
	ID3D11ShaderResourceView* pTexture = nullptr;		// �e�N�X�`��
	ID3D11ShaderResourceView* pNormalTexture = nullptr;		// �e�N�X�`��
	ePrimitiveType primitiveType;			// �v���~�e�B�u�^
	EBlendState blendState;
	float fAlpha;							// ���_�J���[���ŏ��l

	XMFLOAT4X4* mtxTexture = nullptr;					// �e�N�X�`�� �}�g���b�N�X
	MATERIAL material;					// �}�e���A��
	bool bBump = false;
	bool bBillboard = false;
} ;

struct InstancingMeshData {
	XMFLOAT4X4* mtxWorld = nullptr;					// ���[���h�}�g���b�N�X
	XMFLOAT3* pPos;
	XMFLOAT3* pRot;
	XMFLOAT3* pScale;
	//XMFLOAT4X4* mtxTexture = nullptr;					// �e�N�X�`�� �}�g���b�N�X
	//MATERIAL material;					// �}�e���A��
	//bool bBump = false;
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitInstancingMesh(void);
void UninitInstancingMesh(void);
void UpdateInstancingMesh(InstancingMesh* pInstancingMesh);
void DrawInstancingMesh(ID3D11DeviceContext* pDeviceContext, InstancingMesh* pInstancingMesh,
	std::vector<InstancingMeshData*>& InstancingList, int nTranslucntType = TT_NOAFFECT);
HRESULT MakeInstancingMeshVertex(ID3D11Device* pDevice, InstancingMesh* pInstancingMesh, VERTEX_3D vertexWk[], int indexWk[]);
void ReleaseInstancingMesh(InstancingMesh* pInstancingMesh);
void SetInstancingMeshCamera(CCamera* pCamera);
void SetInstancingMeshLight(CLight* pLight);
HRESULT UpdateInstancingMeshVertex(ID3D11DeviceContext* pDeviceContext, InstancingMesh* pInstancingMesh, VERTEX_3D vertex[]);
void DrawInstancingMeshShadow(ID3D11DeviceContext* pDeviceContext, InstancingMesh* pInstancingMesh,
	std::vector<InstancingMeshData*>& InstancingList, int nTranslucntType = TT_NOAFFECT);


