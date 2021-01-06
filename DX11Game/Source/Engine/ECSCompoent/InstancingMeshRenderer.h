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
		void SetDiffuseColor(XMFLOAT4 color) { m_data.material.Diffuse = color; }

		// ベースカラーテクスチャのセット
		void SetDiffuseTexture(const char* filename);
		// ベースカラーテクスチャのセット
		void SetDiffuseTexture(const wchar_t* filename);

		// プリミティブ
		void SetPrimitiveType(ePrimitiveType eType) { if (m_mesh) m_mesh->primitiveType = eType; }

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
