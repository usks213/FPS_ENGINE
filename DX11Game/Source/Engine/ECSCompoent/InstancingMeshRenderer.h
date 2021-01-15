//==================================================================
//								InstancingMeshRenderer.h
//	インスタンシングメッシュレンダラー
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
#include "../Renderer/instancingMesh.h"
#include <string>


//===== マクロ定義 =====


//===== クラス定義 =====

namespace ECS
{
	class InstancingMeshRenderer : public Renderer
	{
	public:
		// コンストラクタ
		InstancingMeshRenderer();
		// デストラクタ
		~InstancingMeshRenderer();

		// オブジェクト生成時
		void OnCreate() override;
		// オブジェクト破棄時
		void OnDestroy() override;

		// 各レンダラーの描画処理
		void EarlyDraw(ID3D11DeviceContext* pDC) override;
		void LateDraw(ID3D11DeviceContext* pDC) override;

		// カメラからの距離
		void LayerUpdate() override {}

		// スプライトメッシュ
		HRESULT MakeSprite(const std::string tag);
		// 平面メッシュの生成
		HRESULT MakePlane(const std::string tag, int nNumBlockX, int nNumBlockZ, float fSizeBlockX, float fSizeBlockZ,
			float fTexSizeX = 1.0f, float fTexSizeZ = 1.0f);
		// キューブ
		HRESULT MakeCube(const std::string tag);
		// スフィア
		HRESULT MakeSphere(std::string tag,
			int nSegment = 36, float fTexSplit = 1.0f, XMFLOAT3 pos = { 0,0,0 });
		// スフィア
		HRESULT MakeSphere2(std::string tag,
			int nNumBlockX, int nNumBlockY, float fSize = 0.5f, 
			float fTexSizeX = 1.0f, float fTexSizeY = 1.0f, 
			float fPosX = 0.0f, float fPosY = 0.0f, float fPosZ = 0.0f);
		// 正四面体
		HRESULT MakeTetraheron(const std::string tag);
		// 八面体 Octahedron
		HRESULT MakeOctahedron(const std::string tag);
		// 十二面体
		HRESULT MakeDodecahedron(const std::string tag);
		// 二十面体 Icosahedron
		HRESULT MakeIcosahedron(const std::string tag);

		//  s08
		HRESULT MakeS08(const std::string tag);
		//  n15
		HRESULT MakeN15(const std::string tag);
		//  s06
		HRESULT MakeS06(const std::string tag);

		//--- マテリアル
		// カラー変更
		void SetDiffuseColor(XMFLOAT4 color) { if (m_mesh) m_mesh->material.Diffuse = color; }
		// ビルボード
		void SetBillboard(bool bFlg) { if (m_mesh) m_mesh->bBillboard = bFlg; }
		// ブレンドステート
		void SetBlendStateData(EBlendState eState) { if (m_mesh) m_mesh->blendState = eState; }

		// ベースカラーテクスチャのセット
		void SetDiffuseTexture(const char* filename);
		// ベースカラーテクスチャのセット
		void SetDiffuseTexture(const wchar_t* filename);

		// プリミティブ
		void SetPrimitiveType(ePrimitiveType eType) { if (m_mesh) m_mesh->primitiveType = eType; }

	private:
		// メッシュデータの生成
		bool CreateMeshData(std::string tag);

	private:
		// メッシュデータ
		InstancingMesh* m_mesh;
		// インスタンシングデータ
		InstancingMeshData m_data;

		// メッシュのプール
		static std::map<const std::string, InstancingMesh*, std::less<>> m_meshPool;
		static std::map<const std::string, std::vector<InstancingMeshData*>, std::less<>> m_meshList;

		// メッシュタグ
		std::string m_tag;
	};
}
