//======================================================================
//											PostProcessing.h
//	ポストエフェクト
//
//======================================================================
//	author :	AT12A 05 宇佐美晃之
//======================================================================
//	開発履歴
//
//	2021/01/13	作成
//
//======================================================================
#pragma once


//====== インクルード部 ======
#include "../main.h"


//===== クラス定義 =====
class PostProcessing
{
public:
	PostProcessing();
	~PostProcessing();

	HRESULT Init();
	HRESULT Uninit();
	HRESULT Draw();

public:
	ID3D11VertexShader* m_pVertexShader;		// 頂点シェーダ
	ID3D11PixelShader*	m_pPixelShader;			// ピクセルシェーダ
	ID3D11InputLayout*	m_pInputLayout;			// 頂点フォーマット


	ID3D11RenderTargetView*		m_pRenderTargetView;	// レンダーターゲット
	ID3D11SamplerState*			m_pSamplerState;		// テクスチャ サンプラ
	ID3D11Texture2D*			m_pRenderTexture;		// レンダーテクスチャ
	ID3D11ShaderResourceView*	m_pTexureResourceView;	// テクスチャリソース

	ID3D11Buffer* m_pVertexConstantBuffer;		// 頂点定数バッファ
	ID3D11Buffer* m_pPixelConstantBuffer;		// 頂点定数バッファ
};


