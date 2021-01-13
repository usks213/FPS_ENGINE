//==================================================================
//												WorldManager.h
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

#pragma once

//====== インクルード部 ======
#include <unordered_map>
#include <map>
#include <string>
#include "World.h"
#include "../../../Scripts/FadeScript.h"
#include "../../ECSEntity/GameObject.h"


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	class WorldManager final
	{
	private:
		// インスタンス
		static WorldManager* g_pInstance;

		// コンストラクタ
		WorldManager();
		// デストラクタ
		~WorldManager();

	public:
		// インスタンスの取得
		static WorldManager* GetInstance() { return g_pInstance; }

		// インスタンス生成
		static void Create();
		// インスタンス破棄
		static void Destroy();

	private:
		// ワールドプール
		using WorldPool = std::map<const std::string,std::shared_ptr<World>>;

		// ワールドプール
		WorldPool m_WorldList;
		// ワールド破棄リスト
		std::unordered_map<const World*, WorldPool::iterator> m_DestroyList;

		// マネージャーが持っている唯一のワールド
		std::unique_ptr<World> m_masterWorld;
		// 現在のシーン
		std::weak_ptr<World> m_currentWorld;
		// 次のワールド
		std::weak_ptr<World> m_nextWorld;

		// ワールド変更フラグ
		bool m_bChange;

	public:
		// 初期化
		void Start();
		// 終了
		void End();
		// 更新
		void Update();
		// 描画
		void Draw();

		// ワールドの生成
		template<class T> 
		std::shared_ptr<T> CreateWorld(const std::string name);
		// ワールドの追加
		void AddWorld(const std::shared_ptr<World>& world ) {
			m_WorldList.emplace(world->m_name, world);}

		// ワールドの破棄
		void DestroyWorld(const std::string name);
		void DestroyWorld(const std::shared_ptr<World>& obj);
		void DestroyWorld(World* obj);

		// デストロイリストのクリア
		void ClearnUpWorld();

		// マスターワールド取得
		World* GetMasterWorld() { return m_masterWorld.get(); }

		// ワールド切替
		template <class T>
		void LoadWorld(const std::string name);
		// 次のワールドに変更
		void SetNextWorld();

	};

	// ワールドの生成
	template<class T>
	std::shared_ptr<T> WorldManager::CreateWorld(const std::string name)
	{
		// 検索
		auto itr = m_WorldList.find(name);
		// プールにあった
		if (m_WorldList.end() != itr) 
			return std::static_pointer_cast<T>(itr->second);

		// 生成
		std::shared_ptr<T> ptr(new T(name));

		// 格納
		m_WorldList.emplace(name, ptr);

		return ptr;
	}

	template <class T>
	void WorldManager::LoadWorld(const std::string name)
	{
		// ワールド変更中
		if (m_bChange) return;

		// 次のワールドの生成
		m_nextWorld = CreateWorld<T>(name);

		// 同じワールドがあったら
		if (m_currentWorld.lock().get() == m_nextWorld.lock().get())
		{
			// ワールドをリロード
			std::shared_ptr<T> ptr(new T(name));
			m_nextWorld = ptr;

			// ワールド変更フラグオン
			m_bChange = true;

			// フェードの生成
			const auto& obj = m_masterWorld->GetEntityManager()->CreateEntity<GameObject>();
			const auto& fade = obj->AddComponent<FadeScript>();
			fade->SetFadeEndFunc([this, ptr]() 
				{
				this->SetNextWorld(); 
				this->AddWorld(ptr);
				});
			// フェードの実行
			fade->StartFadeOut();

			return;
		}

		// ワールド変更フラグオン
		m_bChange = true;

		// フェードの生成
		const auto& obj = m_masterWorld->GetEntityManager()->CreateEntity<GameObject>();
		const auto& fade = obj->AddComponent<FadeScript>();
		fade->SetFadeEndFunc([this]() {this->SetNextWorld(); });
		// フェードの実行
		fade->StartFadeOut();

		// Test 
		//SetNextWorld();
	}
}
