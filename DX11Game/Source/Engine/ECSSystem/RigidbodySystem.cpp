//==================================================================
//												RigidbodySystem.cpp
//	リジッドボディ管理システム
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/27	リジッドボディシステム作成
//
//===================================================================


//===== インクルード部 =====
#include "RigidbodySystem.h"
#include "../ECSCompoent/Rigidbody.h"
#include <algorithm>

using namespace ECS;


//===== マクロ定義 =====


//===== プロトタイプ宣言 =====


//===== グローバル変数 =====



//===================================
//
//	コンストラクタ
//
//===================================
RigidbodySystem::RigidbodySystem(World* pWorld)
	: System<Rigidbody>(pWorld)
{
	// 更新順
	SetUpdateOrder(UpdateOrder::eRigidbody);
}


//===================================
//
//	デストラクタ
//
//===================================
RigidbodySystem::~RigidbodySystem()
{
}


//===================================
//
//	生成時コールバック
//
//===================================
void RigidbodySystem::OnCreate()
{
}

//===================================
//
//	更新時コールバック
//
//===================================
void RigidbodySystem::OnUpdate()
{
	std::for_each(m_ComponentList.begin(), m_ComponentList.end(),
		[](Rigidbody* rb)
		{
			rb->Update();
		});
}

//===================================
//
//	後更新時コールバック
//
//===================================
void RigidbodySystem::OnLateUpdate()
{
	std::for_each(m_ComponentList.begin(), m_ComponentList.end(),
		[](Rigidbody* rb)
		{
			rb->LateUpdate();
		});
}

//===================================
//
//	削除時コールバック
//
//===================================
void RigidbodySystem::OnDestroy()
{
}
