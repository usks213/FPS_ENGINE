//=============================================================================
//
// メッシュ共通処理 [mesh.h]
// Author : KOSHI USAMI
//
//=============================================================================
#pragma once

#include "../main.h"
#include "Camera.h"
#include "Light.h"

//*****************************************************************************
// 定数定義
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
	TT_NOAFFECT = 0,	// 全て
	TT_OPACITYONLY,		// 不透明のみ
	TT_TRANSLUCENTONLY,	// 半透明のみ

	MAX_TRANSLUCENTTYPE
};

//**************************************
// 構造体定義
//**************************************
struct MATERIAL {
	XMFLOAT4	Diffuse  =	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);;		// Diffuse color RGBA
	XMFLOAT4	Ambient	 =	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);;		// Ambient color RGB
	XMFLOAT4	Specular =	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);;		// Specular 'shininess'
	XMFLOAT4	Emissive =	XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);;		// Emissive color RGB
	float		Power	 =	50.0f;;			// Sharpness if specular highlight
} ;

struct MESH {
	ID3D11Buffer* pVertexBuffer = nullptr;			// 頂点バッファインターフェースへのポインタ
	ID3D11Buffer* pIndexBuffer = nullptr;				// インデックスバッファインターフェースへのポインタ

	XMFLOAT4X4 mtxWorld;					// ワールドマトリックス
	//XMFLOAT3 pos;							// ポリゴン表示位置の中心座標
	//XMFLOAT3 rot;							// ポリゴンの回転角
	int nNumVertex = 0;							// 総頂点数	
	int nNumIndex = 0;							// 総インデックス数

	XMFLOAT4X4 mtxTexture;					// テクスチャ マトリックス
	ID3D11ShaderResourceView* pTexture = nullptr;			// テクスチャ
	ID3D11ShaderResourceView* pNormalTexture = nullptr;		// 法線テクスチャ
	ID3D11ShaderResourceView* pAmbientTexture = nullptr;	// 環境テクスチャ
	bool bLight = true;
	ePrimitiveType primitiveType;			// プリミティブ型

	MATERIAL* pMaterial = nullptr;					// マテリアル
	float fAlpha;							// 頂点カラーα最小値
} ;

//*****************************************************************************
// プロトタイプ宣言
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


extern ID3D11DepthStencilView*		g_pShadowDepthStencilView;	// Zバッファ
extern ID3D11SamplerState*			g_pShadowSamplerState;		// テクスチャ サンプラ
extern ID3D11Texture2D*				g_pShadowMap;				// シャドウマップテクスチャ
extern ID3D11ShaderResourceView*	g_pShadowResourceView;		// シャドウマップリソース


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
	ID3D11Buffer* pVertexBuffer;			// 頂点バッファインターフェースへのポインタ
	ID3D11Buffer* pIndexBuffer;				// インデックスバッファインターフェースへのポインタ

	XMFLOAT4X4 mtxWorld;					// ワールドマトリックス
	XMFLOAT3 pos;							// ポリゴン表示位置の中心座標
	XMFLOAT3 rot;							// ポリゴンの回転角
	int nNumVertex;							// 総頂点数	
	int nNumIndex;							// 総インデックス数

	XMFLOAT4X4 mtxTexture;					// テクスチャ マトリックス
	ID3D11ShaderResourceView* pTexture;		// テクスチャ
	ID3D11ShaderResourceView* pNormalTexture = nullptr;		// テクスチャ
	bool bBump;

	ePrimitiveType primitiveType;			// プリミティブ型

	MATERIAL* pMaterial;					// マテリアル
	float fAlpha;
};

