//==================================================================
//												ECSRigidbodySystem.h
//	データ指向版リジッドボディ管理システム
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/01/05	データ指向版で作成
//
//===================================================================


//===== インクルード部 =====
#include "ECSRigidbodySystem.h"
#include "../ECSCompoent/ECSRigidbody.h"
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
ECSRigidbodySystem::ECSRigidbodySystem(World* pWorld)
	: ECSSystem<RigidbodyData>(pWorld)
{
	// 更新順
	SetUpdateOrder(UpdateOrder::eRigidbody);
}


//===================================
//
//	デストラクタ
//
//===================================
ECSRigidbodySystem::~ECSRigidbodySystem()
{
}


//===================================
//
//	生成時コールバック
//
//===================================
void ECSRigidbodySystem::OnCreate()
{
}

//===================================
//
//	更新時コールバック
//
//===================================
void ECSRigidbodySystem::OnUpdate()
{
	std::for_each(m_ComponentList.begin(), m_ComponentList.end(),
		[](RigidbodyData& rb)
		{
			rb.Update();
		});
}

//===================================
//
//	後更新時コールバック
//
//===================================
void ECSRigidbodySystem::OnLateUpdate()
{
	std::for_each(m_ComponentList.begin(), m_ComponentList.end(),
		[](RigidbodyData& rb)
		{
			rb.LateUpdate();
		});
}

//===================================
//
//	削除時コールバック
//
//===================================
void ECSRigidbodySystem::OnDestroy()
{
}
