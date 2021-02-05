//=============================================================================
//
// メイン処理 [main.h]
// Author : KOSHI USAMI
//
//=============================================================================
#pragma once

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#define NOMINMAX
#include <windows.h>
#include <tchar.h>
#include <math.h>
#include <mmsystem.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "resource.h"

// 本来はヘッダに書かない方が良い
using namespace DirectX;

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) {if(x){(x)->Release();x=nullptr;}}
#endif
#ifndef SAFE_DELETE
#define SAFE_DELETE(x) {if(x){delete(x);x=nullptr;}}
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x) {if(x){delete[](x);x=nullptr;}}
#endif

//#define SCREEN_WIDTH	(1920)				// ウインドウの幅
//#define SCREEN_HEIGHT	(1080)				// ウインドウの高さ

#define SCREEN_WIDTH	(1280)				// ウインドウの幅
#define SCREEN_HEIGHT	( 720)				// ウインドウの高さ

#define SCREEN_CENTER_X	(SCREEN_WIDTH/2)	// ウインドウの中心Ｘ座標
#define SCREEN_CENTER_Y	(SCREEN_HEIGHT/2)	// ウインドウの中心Ｙ座標

#define SCREEN_SCALE_X (SCREEN_WIDTH  / 1920.0f)
#define SCREEN_SCALE_Y (SCREEN_HEIGHT / 1080.0f)

#define	NUM_VERTEX		(4)					// 頂点数
#define	NUM_POLYGON		(2)					// ポリゴン数

#define SinDeg(degree)	sinf(XMConvertToRadians(degree))
#define CosDeg(degree)	cosf(XMConvertToRadians(degree))

enum EBlendState {
	BS_NONE = 0,							// 半透明合成無し
	BS_ALPHABLEND,							// 半透明合成
	BS_ADDITIVE,							// 加算合成
	BS_SUBTRACTION,							// 減算合成

	MAX_BLENDSTATE
};

enum ECullMode {
	CULLMODE_NONE = 0,						// カリングしない
	CULLMODE_CW,							// 前面カリング
	CULLMODE_CCW,							// 背面カリング

	MAX_CULLMODE
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
// 頂点フォーマット( 頂点座標[2D] / 反射光 / テクスチャ座標 )
typedef struct {
	XMFLOAT3 vtx;		// 頂点座標
	XMFLOAT4 diffuse;	// 拡散反射光
	XMFLOAT2 tex;		// テクスチャ座標
} VERTEX_2D;

// 頂点フォーマット( 頂点座標[3D] / 法線ベクトル / 反射光 / テクスチャ座標 )
typedef struct {
	XMFLOAT3 vtx;		// 頂点座標
	XMFLOAT3 nor;		// 法線ベクトル
	XMFLOAT4 diffuse;	// 拡散反射光
	XMFLOAT2 tex;		// テクスチャ座標
} VERTEX_3D;


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HWND GetMainWnd();
HINSTANCE GetInstance();
ID3D11Device* GetDevice();
ID3D11DeviceContext* GetDeviceContext();
void SetZBuffer(bool bEnable);
void SetZWrite(bool bEnable);
void SetBlendState(int nBlendState);
void SetCullMode(int nCullMode);
void SetRenderTarget();
DXGI_SAMPLE_DESC GetMSAA();