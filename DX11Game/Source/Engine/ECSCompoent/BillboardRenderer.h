//==================================================================
//								BillboardRenderer.h
//	ビルボードレンダラー
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
#include "../Renderer/mesh.h"


//===== マクロ定義 =====


//===== クラス定義 =====

namespace ECS
{
	class BillboardRenderer : public Renderer
	{
	public:
		// コンストラクタ
		BillboardRenderer();
		// デストラクタ
		~BillboardRenderer();

		// 各レンダラーの描画処理
		void EarlyDraw(ID3D11DeviceContext* pDC) override;
		void LateDraw(ID3D11DeviceContext* pDC) override;

		// ビルボードメッシュ取得
		static MESH* GetBillboardMesh();

		//--- マテリアル
		// カラー変更
		void SetDiffuseColor(XMFLOAT4 color) { m_material.Diffuse = color; }

		// ベースカラーテクスチャのセット
		void SetDiffuseTexture(const char* filename);
		// ベースカラーテクスチャのセット
		void SetDiffuseTexture(const wchar_t* filename);

		// 描画タイプ
		void SetTranslucentType(eTranslucentType eType) { m_eTranslucentType = eType; }
		// プリミティブ
		void SetPrimitiveType(ePrimitiveType eType) { if (m_mesh) m_mesh->primitiveType = eType; }

	private:
		// メッシュデータ
		MESH* m_mesh;
		// マテリアルデータ
		MATERIAL m_material;

		// 描画タイプ
		eTranslucentType m_eTranslucentType;
		// テクスチャ
		ID3D11ShaderResourceView* m_diffuseTexture;

		// メッシュ
		static MESH m_masterMesh;
	};
}
