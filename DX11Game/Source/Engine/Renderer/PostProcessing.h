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

	HRESULT Init(ID3D11Device* pDevice);
	void Uninit();
	void Draw(ID3D11DeviceContext* pDeviceContext);

	static PostProcessing* GetMain() { return m_pMain; }

	XMFLOAT4* GetColor() { return &m_col; }
public:
	ID3D11VertexShader* m_pVertexShader;		// 頂点シェーダ
	ID3D11PixelShader*	m_pPixelShader;			// ピクセルシェーダ
	ID3D11InputLayout*	m_pInputLayout;			// 頂点フォーマット

	ID3D11Buffer*		m_pVertexConstantBuffer;		// 頂点シェーダー定数バッファ
	ID3D11Buffer*		m_pPixelConstantBuffer;			// ピクセルシェーダー定数バッファ
	ID3D11SamplerState* m_pSamplerState;				// テクスチャ サンプラ
	ID3D11Buffer*		m_pVertexBuffer;				// 頂点バッファ

	ID3D11RenderTargetView*		m_pRenderTargetView;	// レンダーターゲット
	ID3D11Texture2D*			m_pRenderTexture;		// レンダーテクスチャ
	ID3D11ShaderResourceView*	m_pTexureResourceView;	// テクスチャリソース

	ID3D11DepthStencilView*		m_pDepthStencilView;		// Zバッファ
	ID3D11SamplerState*			m_pDepthSamplerState;		// テクスチャ サンプラ
	ID3D11Texture2D*			m_pDepthStencTexure;		// シャドウマップテクスチャ
	ID3D11ShaderResourceView*	m_pDepthStencResourceView;	// シャドウマップリソース

private:
	VERTEX_2D	m_vertexWk[NUM_VERTEX] = {};	// 頂点情報格納ワーク

	XMFLOAT3	m_pos;			// ポリゴンの移動量
	XMFLOAT3	m_rot;			// ポリゴンの回転量
	XMFLOAT3	m_siz;			// ポリゴンのサイズ
	XMFLOAT4	m_col;			// ポリゴンの頂点カラー
				
	XMFLOAT2	m_posTex;		// UV座標
	XMFLOAT2	m_sizTex;		// テクスチャ抽出サイズ
				
	XMFLOAT4X4	m_mProj;				// 射影変換行列
	XMFLOAT4X4	m_mView;				// ビュー変換行列
	XMFLOAT4X4	m_mWorld;				// ワールド変換行列
	XMFLOAT4X4	m_mTex;					// テクスチャ変換行列

private:
	HRESULT MakeVertex(ID3D11Device* pDevice);

	static PostProcessing* m_pMain;
};


