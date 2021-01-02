//==================================================================
//								SpriteRenderer.h
//	スプライトレンダラー
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/02	ECS用に再設計
//
//===================================================================

#pragma once

//====== インクルード部 ======
#include "Renderer.h"
#include <map>
#include <string>
#include "../Renderer/Camera.h"


//===== マクロ定義 =====


//===== クラス定義 =====

namespace ECS
{
	class SpriteRenderer : public Renderer
	{
	public:
		// レイヤー
		enum ESpriteLayer
		{
			// 先頭
			eTop,

			// フェード
			eFade3 = 1,
			eFade2,
			eFade1,

			// UI
			eUI5 = 5,
			eUI4,
			eUI3,
			eUI2,
			eUI1,

			eDefault,

			// 背景
			eBG3 = (int)VIEW_FAR_Z + 1,
			eBG2,
			eBG1,

			// 最後
			eBottom,

			eMaxLayer,
		};
	public:
		// コンストラクタ
		SpriteRenderer();
		// デストラクタ
		~SpriteRenderer();

		// 各レンダラーの描画処理
		void EarlyDraw(ID3D11DeviceContext* pDC) override;
		void LateDraw(ID3D11DeviceContext* pDC) override;

		// カメラからの距離を格納
		virtual void LayerUpdate(){}

		//--- マテリアル
		// カラー変更
		void SetDiffuseColor(Vector3 color) { m_color = color; }
		void SetDiffuseColor(float r, float g, float b) {
			m_color->x = r;
			m_color->y = g;
			m_color->z = b;
		}
		void SetDiffuseColor(float r, float g, float b, float a) {
			m_color->x = r;
			m_color->y = g;
			m_color->z = b;
			m_fAlpha = a;
		}
		void SetAlpha(float alpha) { m_fAlpha = alpha; }

		// テクスチャのセット
		void SetDiffuseTexture(const char* filename);
		void SetDiffuseTexture(const wchar_t* filename);

		// レイヤー
		void SetLayer(ESpriteLayer eLayer) { m_fLayer = (float)eLayer; }
		void SetLayer(int nLayer) { m_fLayer = (float)nLayer; }


		// カラー取得
		Vector3 GetColor() { return m_color; }
		// アルファ取得
		float GetAlpha() { return m_fAlpha; }
		// レイヤー取得
		int GetLayer() { return (int)m_fLayer; }

	private:
		// テクスチャ
		ID3D11ShaderResourceView* m_pTexture;
		// カラー
		Vector3 m_color;
		// アルファ
		float m_fAlpha;
	};
}
