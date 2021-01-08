//==================================================================
//												ObjectManager.h
//	オブジェクトの管理クラス
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/21	オブジェクトマネージャークラス作成
//
//===================================================================

#pragma once

//====== インクルード部 ======
#include <list>
#include <map>
#include <unordered_map>
#include "IObject.h"


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	class ObjectManager final
	{
	private:
		// インスタンス
		static ObjectManager* g_pInstance;

		// コンストラクタ
		ObjectManager();
		// デストラクタ
		~ObjectManager();

	public:
		// インスタンスの取得
		static ObjectManager* GetInstance() { return g_pInstance; }

		// インスタンス生成
		static void Create();
		// インスタンス破棄
		static void Destroy();


	private:
		// オブジェクトプール
		using ObjectPool = std::unordered_map<const IObject*,std::shared_ptr<IObject>>;

		// オブジェクトプール
		ObjectPool m_ObjectList;
		// オブジェクト破棄リスト
		std::unordered_map<const IObject*, ObjectPool::iterator> m_DestroyList;

	public:
		// オブジェクトの生成
		template<class T> 
		void AddObjectPool(const std::shared_ptr<T>& obj);

		// オブジェクトの破棄
		void DestroyObject(const std::shared_ptr<IObject>& obj);
		void DestroyObject(IObject* obj);

		// デストロイリストのクリア
		void ClearnUpObject();

	};

	// オブジェクトの生成
	template<class T>
	void ObjectManager::AddObjectPool(const std::shared_ptr<T>& ptr)
	{
		// 格納
		//m_ObjectList.push_back(ptr);
		m_ObjectList.emplace(ptr.get(), ptr);

		// 自身を格納
		ptr->m_self = ptr;

		// 生成後関数
		ptr->OnCreate();
	}
}
