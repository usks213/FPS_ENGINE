//==================================================================
//								MeshRenderer.h
//	メッシュレンダラー
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/23	ECS用に再設計
//
//===================================================================

#pragma once

//====== インクルード部 ======
#include "Renderer.h"
#include <map>
#include "../Renderer/mesh.h"
#include <string>


//===== マクロ定義 =====


//===== クラス定義 =====

namespace ECS
{
	class MeshRenderer : public Renderer
	{
	public:
		// コンストラクタ
		MeshRenderer();
		// デストラクタ
		~MeshRenderer();

		// 各レンダラーの描画処理
		void EarlyDraw(ID3D11DeviceContext* pDC) override;
		void LateDraw(ID3D11DeviceContext* pDC) override;

		// 平面メッシュの生成
		HRESULT MakePlane(const std::string tag, int nNumBlockX, int nNumBlockZ, float fSizeBlockX, float fSizeBlockZ,
			float fTexSizeX = 1.0f, float fTexSizeZ = 1.0f);
		// キューブ
		HRESULT MakeCube(const std::string tag);
		// スフィア
		HRESULT MakeSphere(std::string tag,
			int nSegment = 36, float fTexSplit = 1.0f, XMFLOAT3 pos = { 0,0,0 });

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

		// メッシュのプール
		static std::map<const std::string, MESH*, std::less<>> m_meshPool;
	};
}
