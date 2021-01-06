//==================================================================
//												RendererSystem.h
//	レンダラー管理システム
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/23	レンダラーシステム作成
//
//===================================================================


//===== インクルード部 =====
#include "RendererSystem.h"
#include "../ECSCompoent/Renderer.h"
#include <algorithm>

#include "../main.h"


using namespace ECS;


//===== マクロ定義 =====


//===== プロトタイプ宣言 =====


//===== グローバル変数 =====



//===================================
//
//	コンストラクタ
//
//===================================
RendererSystem::RendererSystem(World* pWorld)
	: System<Renderer>(pWorld)
{
	// 更新順
	SetUpdateOrder(UpdateOrder::eRenderer);
}


//===================================
//
//	デストラクタ
//
//===================================
RendererSystem::~RendererSystem()
{
}


//===================================
//
//	生成時コールバック
//
//===================================
void RendererSystem::OnCreate()
{
}

//===================================
//
//	描画時コールバック
//
//===================================
void RendererSystem::OnDraw()
{
	// デバイスコンテキスト
	ID3D11DeviceContext* pDC = GetDeviceContext();

	// レイヤー更新
	std::for_each(m_ComponentList.begin(), m_ComponentList.end(),
		[](Renderer* trans)
		{
			trans->LayerUpdate();
		});

	// ソート
	//std::sort(m_ComponentList.begin(), m_ComponentList.end(), 
	//	[](Renderer* lhs, Renderer* rhs) 
	//	{
	//		// 降順
	//		return lhs->GetLayer() > rhs->GetLayer();
	//	}); 

	// 前描画
	std::for_each(m_ComponentList.begin(), m_ComponentList.end(),
		[&pDC](Renderer* trans)
		{
			trans->EarlyDraw(pDC);
		});

	// 後描画
	std::for_each(m_ComponentList.begin(), m_ComponentList.end(),
		[&pDC](Renderer* trans)
		{
			trans->LateDraw(pDC);
		});
}

//===================================
//
//	削除時コールバック
//
//===================================
void RendererSystem::OnDestroy()
{
}
