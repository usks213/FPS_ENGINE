//==================================================================
//												EntityManager.h
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

#pragma once

//====== インクルード部 ======
#include <list>
#include "IEntity.h"


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	class EntityManager final
	{
	public:
		// コンストラクタ
		EntityManager();
		// デストラクタ
		~EntityManager();

	private:
		// エンティティプール
		using EntityPool = std::list<std::weak_ptr<IEntity>>;

		// エンティティプール
		EntityPool m_EntityList;

	public:
		// エンティティの生成
		template<class T> 
		std::shared_ptr<T> CreateEntity();

		// エンティティの破棄
		void DestroyEntity(const std::shared_ptr<IEntity>& obj);
		void DestroyEntity(IEntity* obj);

		// エンティティの格納
		void AddEntityPool(const std::weak_ptr<IEntity>& entity);
		// エンティティの除外
		void RemoveEntityPool(const std::weak_ptr<IEntity>& entity);
	};

	// エンティティの生成
	template<class T>
	std::shared_ptr<T> EntityManager::CreateEntity()
	{
		// エンティティの生成
		const auto& ptr = std::shared_ptr<T>(new T(this));

		// エンティティプールに登録
		AddEntityPool(ptr);

		// オブジェクトプールに登録
		ObjectManager::GetInstance()->AddObjectPool(ptr);

		return ptr;
	}
}
