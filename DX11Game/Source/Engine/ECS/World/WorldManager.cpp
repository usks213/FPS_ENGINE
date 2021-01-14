//==================================================================
//												WorldManager.cpp
//	ワールドの管理クラス
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/11	ワールドマネージャークラス作成
//
//===================================================================


//===== インクルード部 =====
#include "WorldManager.h"
#include <algorithm>
#include "../Object/ObjectManager.h"
#include "../../ECSWorld/MasterWorld.h"


using namespace ECS;


//===== マクロ定義 =====


//===== プロトタイプ宣言 =====


//===== グローバル変数 =====
WorldManager* WorldManager::g_pInstance = nullptr;



//*************************** シングルトン ***************************

//===================================
//
//	コンストラクタ
//
//===================================
WorldManager::WorldManager()
	:m_masterWorld(new MasterWorld("MasterWorld")),
	m_bChange(false)
{
}


//===================================
//
//	デストラクタ
//
//===================================
WorldManager::~WorldManager()
{
}


//===================================
//
//	インスタンス生成
//
//===================================
void WorldManager::Create()
{
	// 確認
	if (nullptr != g_pInstance) return;

	// 生成
	g_pInstance = new WorldManager();
	g_pInstance->Start();
}


//===================================
//
//	インスタンス破棄
//
//===================================
void WorldManager::Destroy()
{
	// 確認
	if (nullptr == g_pInstance) return;

	// 破棄
	g_pInstance->End();
	delete g_pInstance;
	g_pInstance = nullptr;
}


//*************************** インスタンス ***************************

//===================================
//
//	ワールドマネージャーの初期化
//
//===================================
void WorldManager::Start()
{
	// マスターワールドの初期化
	if(m_masterWorld)
		m_masterWorld->Start();

	// 現在のワールドの初期化
	const auto& world = m_currentWorld.lock();
	if (world) world->Start();
}

//===================================
//
//	ワールドマネージャーの終了
//
//===================================
void WorldManager::End()
{
	// 現在のワールドの終了
	const auto& world = m_currentWorld.lock();
	if (world) world->End();

	// マスターワールドの終了
	if (m_masterWorld)
		m_masterWorld->End();
}

//===================================
//
//	ワールドマネージャーの更新
//
//===================================
void WorldManager::Update()
{
	// 現在のワールドの更新
	const auto& world = m_currentWorld.lock();
	if (world) world->Update();

	// マスターワールドの更新
	if (m_masterWorld)
		m_masterWorld->Update();

	// 現在のワールドの後更新
	if (world) world->LateUpdate();

	// マスターワールドの後更新
	if (m_masterWorld)
		m_masterWorld->LateUpdate();

	// ワールドの削除更新
	ClearnUpWorld();
}

//===================================
//
//	ワールドマネージャーの描画
//
//===================================
void WorldManager::Draw()
{
	// 現在のワールドの描画
	const auto& world = m_currentWorld.lock();
	if (world) world->Draw();

	// マスターワールドの描画
	if (m_masterWorld)
		m_masterWorld->Draw();
}


//===================================
//
//	ワールドの破棄
//
//===================================
void WorldManager::DestroyWorld(const std::string name)
{
	// プールを検索
	auto itr = m_WorldList.find(name);

	// プールになかった
	if (m_WorldList.end() == itr) return;

	// デストロイリストを検索
	auto destroyItr = m_DestroyList.find(itr->second.get());

	// 既に格納されていたら
	if (m_DestroyList.end() != destroyItr) return;

	// デストロイリストに格納
	m_DestroyList.emplace(itr->second.get(), itr);
}

//===================================
//
//	ワールドの破棄
//
//===================================
void WorldManager::DestroyWorld(const std::shared_ptr<World>& obj)
{
	// 名前で検索
	DestroyWorld(obj.get());
}

//===================================
//
//	ワールドの破棄
//
//===================================
void WorldManager::DestroyWorld(World* obj)
{
	// プールを検索
	auto itr = std::find_if(m_WorldList.begin(), m_WorldList.end(),
		[&obj](const auto& map)
		{
			return map.second.get() == obj;
		});

	// プールになかった
	if (m_WorldList.end() == itr) return;

	// デストロイリストを検索
	auto destroyItr = m_DestroyList.find(itr->second.get());

	// 既に格納されていたら
	if (m_DestroyList.end() != destroyItr) return;

	// デストロイリストに格納
	m_DestroyList.emplace(itr->second.get(), itr);
}

//===================================
//
//	デストロイリストのクリア(ワールドの破棄)
//
//===================================
void WorldManager::ClearnUpWorld()
{
	// ワールドの破棄
	for (auto obj : m_DestroyList)
	{
		//削除時実行関数
		obj.second->second->OnDestroy();

		// 完全消去
		m_WorldList.erase(obj.second);
	}

	// リストをクリア
	m_DestroyList.clear();

	// オブジェクトの削除更新
	ObjectManager::GetInstance()->ClearnUpObject();
}

//===================================
//
//	次のワールドに変更
//
//===================================
void WorldManager::SetNextWorld()
{
	if (!m_currentWorld.expired())
	{
		// 現在のシーンの終了処理を呼ぶ
		m_currentWorld.lock()->End();
		// 現在のシーンの消去
		DestroyWorld(m_currentWorld.lock());
	}

	// ワールドのクリーンアップ
	ClearnUpWorld();

	if (!m_nextWorld.expired())
	{
		// 現在のシーンの変更
		m_currentWorld = m_nextWorld;
		// 次のシーンをからに
		m_nextWorld.reset();
		// 次のシーンの初期化
		m_currentWorld.lock()->Start();
	}

	// シーン変更フラグオフ
	m_bChange = false;
}