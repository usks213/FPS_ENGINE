//==================================================================
//												ObjectManager.cpp
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


//===== インクルード部 =====
#include "ObjectManager.h"
#include <algorithm>

using namespace ECS;


//===== マクロ定義 =====


//===== プロトタイプ宣言 =====


//===== グローバル変数 =====
ObjectManager* ObjectManager::g_pInstance = nullptr;



//===================================
//
//	コンストラクタ
//
//===================================
ObjectManager::ObjectManager()
{
}


//===================================
//
//	デストラクタ
//
//===================================
ObjectManager::~ObjectManager()
{
}


//===================================
//
//	インスタンス生成
//
//===================================
void ObjectManager::Create()
{
	// 確認
	if (nullptr != g_pInstance) return;

	// 生成
	g_pInstance = new ObjectManager();
}


//===================================
//
//	インスタンス破棄
//
//===================================
void ObjectManager::Destroy()
{
	// 確認
	if (nullptr == g_pInstance) return;

	// 破棄
	delete g_pInstance;
	g_pInstance = nullptr;
}


//===================================
//
//	オブジェクトの破棄
//
//===================================
void ObjectManager::DestroyObject(const std::shared_ptr<IObject>& obj)
{
	// プールを検索
	//auto itr = std::find(m_ObjectList.begin(), m_ObjectList.end(), obj);
	auto itr = m_ObjectList.find(obj.get());

	// プールになかった
	if (m_ObjectList.end() == itr) return;

	// デストロイリストを検索
	//auto destroyItr = std::find(m_DestroyList.begin(), m_DestroyList.end(), itr);
	auto destroyItr = m_DestroyList.find(obj.get());

	// 既に格納されていたら
	if (m_DestroyList.end() != destroyItr) return;

	// デストロイリストに格納
	//m_DestroyList.push_back(itr);
	m_DestroyList.emplace(obj.get(),itr);
}

//===================================
//
//	オブジェクトの破棄
//
//===================================
void ObjectManager::DestroyObject(IObject* obj)
{
	// プールを検索
	/*auto itr = std::find_if(m_ObjectList.begin(), m_ObjectList.end(),
		[obj](std::shared_ptr<IObject> obj_s)
		{
			return obj_s.get() == obj;
		});*/
	auto itr = m_ObjectList.find(obj);


	// プールになかった
	if (m_ObjectList.end() == itr) return;

	// デストロイリストを検索
	//auto destroyItr = std::find(m_DestroyList.begin(), m_DestroyList.end(), itr);
	auto destroyItr = m_DestroyList.find(obj);

	// 既に格納されていたら
	if (m_DestroyList.end() != destroyItr) return;

	// デストロイリストに格納
	//m_DestroyList.push_back(itr);
	m_DestroyList.emplace(obj, itr);
}


//===================================
//
//	デストロイリストのクリア(オブジェクトの破棄)
//
//===================================
void ObjectManager::ClearnUpObject()
{
	//// オブジェクトの破棄
	//std::for_each(m_DestroyList.begin(), m_DestroyList.end(),
	//	[this](const ObjectPool::iterator& itr)
	//	{
	//		//削除時実行関数
	//		//(*itr)->OnDestroy();
	//		itr->second->OnDestroy();

	//		// 完全消去
	//		m_ObjectList.erase(itr);
	//	});

	// オブジェクトの破棄
	for (auto obj : m_DestroyList)
	{
		//削除時実行関数
		obj.second->second->OnDestroy();

		// 完全消去
		m_ObjectList.erase(obj.second);
	}

	// リストをクリア
	m_DestroyList.clear();
}