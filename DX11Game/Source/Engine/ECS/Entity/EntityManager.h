////==================================================================
////												ObjectManager.h
////	オブジェクトの管理クラス
////
////==================================================================
////	author :	AT12A 05 宇佐美晃之
////==================================================================
////	開発履歴
////
////	2020/12/21	オブジェクトマネージャークラス作成
////
////===================================================================
//
//#pragma once
//
////====== インクルード部 ======
//#include <list>
//#include "Object.h"
//
//
////===== マクロ定義 =====
//
//
////===== クラス定義 =====
//namespace ECS
//{
//	class ObjectManager final
//	{
//	private:
//		// インスタンス
//		static ObjectManager* g_pInstance;
//
//		// コンストラクタ
//		ObjectManager();
//		// デストラクタ
//		~ObjectManager();
//
//	public:
//		// インスタンスの取得
//		static ObjectManager* GetInstance() { return g_pInstance; }
//
//		// インスタンス生成
//		static void Create();
//		// インスタンス破棄
//		static void Destroy();
//
//
//	private:
//		// オブジェクトプール
//		using ObjectPool = std::list<std::shared_ptr<Object>>;
//
//		// オブジェクトプール
//		ObjectPool m_ObjectList;
//		// オブジェクト破棄リスト
//		std::list<ObjectPool::iterator> m_DestroyList;
//
//	public:
//		// オブジェクトの生成
//		template<class T> 
//		std::shared_ptr<T> CreateObject();
//
//		// オブジェクトの破棄
//		void DestroyObject(std::shared_ptr<Object> obj);
//		void DestroyObject(Object* obj);
//
//		// デストロイリストのクリア
//		void ClearnUpObject();
//
//	};
//
//	// オブジェクトの生成
//	template<class T>
//	std::shared_ptr<T> ObjectManager::CreateObject()
//	{
//		// 生成
//		const auto& ptr = std::shared_ptr<T>(new T());
//
//		// 格納
//		m_ObjectList.push_back(ptr);
//
//		// 生成後関数
//		//ptr->OnCreate();
//
//		// 自身を格納
//		ptr->m_self = ptr;
//
//		return ptr;
//	}
//}
