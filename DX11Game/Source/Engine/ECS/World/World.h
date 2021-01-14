//==================================================================
//												World.h
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

#pragma once

//====== インクルード部 ======
#include <vector>
#include <memory>
#include "../uniqueTypeID.h"
#include "../System/ISystem.h"
#include <string>

//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	// 前定義
	class EntityManager;
	class WorldManager;

	// ワールドクラス
	class World
	{
		friend WorldManager;
	private:
		// エンティティマネージャー
		std::unique_ptr<EntityManager> m_pEntityManager;
		// システムリスト
		std::vector<std::unique_ptr<ISystem>> m_SystemList;
		// ワールドネーム
		std::string m_name;

	public:
		// コンストラクタ
		World(std::string name);
		// デストラクタ
		virtual ~World();

		// ワールドの初期化
		virtual void Start(){}
		// ワールドの終了
		virtual void End(){}

		// システムリストの更新
		void Update();
		// システムリストの後更新
		void LateUpdate();
		// システムリストの描画
		void Draw();

		// システムの追加
		template<class T>
		void AddSystem();
		// システムの取得
		template<class T>
		T* GetSystem();

		// エンティティマネージャーを取得
		EntityManager* GetEntityManager() { return m_pEntityManager.get(); }

	private:
		// システムリストのソート
		void sortSystem();
		// OnCreateコールバック関数
		static void CallOnCreate(ISystem* pSystem);

		// 終了時
		void OnDestroy();
	};


	// システムの追加
	template<class T>
	void World::AddSystem()
	{
		// 新規生成
		T* ptr = new T(this);
		// システムIDの登録
		ptr->SetTypeID<T>();
		// リストに追加
		m_SystemList.emplace_back(ptr);
		// 生成コールバック
		CallOnCreate(ptr);
		// ソート
		sortSystem();
	}

	// システムの取得
	template<class T>
	T* World::GetSystem()
	{
		for (const auto& sys : m_SystemList)
		{
			// コンポーネントIDを判定
			if (sys->GetID() == CUniqueTypeID::Get<T>())
			{
				// 見つかったらキャスト
				T* buffer = static_cast<T*>(sys.get());
				return buffer;
			}
		}
		// なかったら
		return nullptr;
	}
}
