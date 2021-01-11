//==================================================================
//												EntityManager.cpp
//	エンティティの管理クラス
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/22	エンティティマネージャークラス作成
//
//===================================================================


//===== インクルード部 =====
#include "EntityManager.h"
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
EntityManager::EntityManager(World* pWorld)
	: m_pWorld(pWorld)
{
}


//===================================
//
//	デストラクタ
//
//===================================
EntityManager::~EntityManager()
{
}


//===================================
//
//	エンティティの破棄
//
//===================================
void EntityManager::DestroyEntity(const std::shared_ptr<IEntity>& entity)
{
	entity->Destroy();
}

//===================================
//
//	エンティティの破棄
//
//===================================
void EntityManager::DestroyEntity(IEntity* entity)
{
	entity->Destroy();
}


//===================================
//
//	エンティティの追加
//
//===================================
void EntityManager::AddEntityPool(const std::weak_ptr<IEntity>& entity)
{
	m_EntityList.push_back(entity);
}


//===================================
//
//	エンティティの除外
//
//===================================
void EntityManager::RemoveEntityPool(const std::weak_ptr<IEntity>& entity)
{
	// 一致するエンティティの検索
	auto itr = std::find_if(m_EntityList.begin(), m_EntityList.end(),
		[entity](const std::weak_ptr<IEntity>& entity2)
		{
			return entity.lock().get() == entity2.lock().get();
		});

	// 見つからなかった
	if (m_EntityList.end() == itr) return;

	// プールから削除
	m_EntityList.erase(itr);
}


//===================================
//
//	エンティティプールのクリア
//
//===================================
void EntityManager::ClearEntityPool()
{
	// 全エンティティの破棄
	for (const auto& entity : m_EntityList)
	{
		DestroyEntity(entity.lock());
	}
}