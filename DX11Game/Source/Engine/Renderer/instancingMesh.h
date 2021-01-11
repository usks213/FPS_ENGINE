//=============================================================================
//
// インスタンシングメッシュ共通処理 [instancingMesh.h]
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
	ID3D11Buffer* pVertexBuffer = nullptr;			// 頂点バッファインターフェースへのポインタ
	ID3D11Buffer* pIndexBuffer = nullptr;				// インデックスバッファインターフェースへのポインタ
	int nNumVertex = 0;							// 総頂点数	
	int nNumIndex = 0;							// 総インデックス数
	ID3D11ShaderResourceView* pTexture = nullptr;		// テクスチャ
	ID3D11ShaderResourceView* pNormalTexture = nullptr;		// テクスチャ
	ePrimitiveType primitiveType;			// プリミティブ型
	EBlendState blendState;
	float fAlpha;							// 頂点カラーα最小値

	XMFLOAT4X4* mtxTexture = nullptr;					// テクスチャ マトリックス
	MATERIAL material;					// マテリアル
	bool bBump = false;
	bool bBillboard = false;
} ;

struct InstancingMeshData {
	XMFLOAT4X4* mtxWorld = nullptr;					// ワールドマトリックス
	XMFLOAT3* pPos;
	XMFLOAT3* pRot;
	XMFLOAT3* pScale;
	//XMFLOAT4X4* mtxTexture = nullptr;					// テクスチャ マトリックス
	//MATERIAL material;					// マテリアル
	//bool bBump = false;
};

//*****************************************************************************
// プロトタイプ宣言
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


