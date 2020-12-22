//==================================================================
//												World.cpp
//	ワールドのベース
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/22	ワールドクラス作成
//
//===================================================================


//===== インクルード部 =====
#include "World.h"
#include "../Entity/EntityManager.h"
#include "../System/ISystem.h"
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
World::World()
{
	// エンティティマネージャーの生成
	m_pEntityManager.reset(new EntityManager(this));
}


//===================================
//
//	デストラクタ
//
//===================================
World::~World()
{
	// システムリストの終了処理
	for (auto&& system : m_SystemList)
	{
		// OnDestroyを呼び出す
		system->OnDestroy();
	}
}


//===================================
//
//	システムリストの更新
//
//===================================
void World::Update()
{
	// システムリストの更新処理
	for (auto&& system : m_SystemList)
	{
		// OnUpdateを呼び出す
		system->OnUpdate();
	}
}


//===================================
//
//	システムリストのソート
//
//===================================
void World::sortSystem()
{
	std::sort(m_SystemList.begin(), m_SystemList.end(),
		[](const std::unique_ptr<ISystem>& lhs, const std::unique_ptr<ISystem>& rhs)
		{
			return lhs->GetUpdateOrder() < rhs->GetUpdateOrder();
		});
}

//= ==================================
//
//	システムOnCreateのコールバック
//
//===================================
void World::CallOnCreate(ISystem* pSystem)
{
	// OnCreateを呼び出す
	pSystem->OnCreate();
}
