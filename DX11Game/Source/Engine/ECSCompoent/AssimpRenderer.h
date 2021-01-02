//==================================================================
//								AssimpRenderer.h
//	アシンプレンダラー
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
#include "../Renderer/AssimpModel.h"
#include <string>


//===== マクロ定義 =====


//===== クラス定義 =====

namespace ECS
{
	class AssimpRenderer : public Renderer
	{
	public:
		// コンストラクタ
		AssimpRenderer();
		// デストラクタ
		~AssimpRenderer();

		// 各レンダラーの描画処理
		void EarlyDraw(ID3D11DeviceContext* pDC) override;
		void LateDraw(ID3D11DeviceContext* pDC) override;

		// モデルのロード
		bool ModelLoad(std::string filename);
		// モデルの解放
		void ModelRelese();
		
		//--- マテリアル
		TAssimpMaterial* GetMaterial();

		// カラー変更
		void SetDiffuseColor(XMFLOAT4 color)
		{
			m_diffuseColor = color;
		}

		// ベースカラーテクスチャのセット
		void SetDiffuseTexture(const char* filename);
		void SetDiffuseTexture(const wchar_t* filename);

		// アニメーションインデックス
		void SetAnimIndex(int nIndex)
		{
			if (m_model)
			{
				m_model->SetAnimIndex(nIndex);
			}
		}

		// 現在のアニメーションインデックスを取得
		int GetCurrentAnimIndex()
		{
			if (m_model)
			{
				return m_model->GetCurrentAnimIndex();
			}
			return -1;
		}

		// アニメーションスピード
		void SetAnimSpeed(double dSpeed)
		{
			if (m_model)
			{
				m_model->SetAnimSpeed(dSpeed);
			}
		}
		// アニメーションの再生場所
		void SetStartAnimTime(double dTime)
		{
			if (m_model)
			{
				m_model->SetStartAnimTime(dTime);
			}
		}

		// アニメーションの再生
		void PlayAnim() {
			if (m_model) {
				m_model->SetStopAnim(false);
			}
		}
		// アニメーションのストップ
		void StopAnim() {
			if (m_model) {
				m_model->SetStopAnim(true);
			}
		}
	private:
		CAssimpModel* m_model;
		std::string m_filename;
		XMFLOAT4 m_diffuseColor;

		// AssimpModelプール
		static std::map<const std::string, CAssimpModel*, std::less<>> m_assimpModelPool;
		// マテリアルプール
		static std::map<const CAssimpModel*, TAssimpMaterial*, std::less<>> m_materialPool;
	};
}
