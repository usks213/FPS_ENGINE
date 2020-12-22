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


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	// 前定義
	class EntityManager;
	class System;

	// ワールドクラス
	class World
	{
	private:
		// エンティティマネージャー
		std::unique_ptr<EntityManager> m_pEntityManager;
		// システムリスト
		std::vector<std::unique_ptr<System>> m_SystemList;

	public:
		// コンストラクタ
		World();
		// デストラクタ
		virtual ~World();

		// システムリストの更新
		void Update();

		// システムの追加
		template<class T>
		void AddSystem()
		{
			// 新規生成
			const auto& ptr = new T(this);
			// リストに追加
			m_SystemList.emplace_back(ptr);
			// 生成コールバック
			CallOnCreate(ptr);
			// ソート
			sortSystem();
		}

		// エンティティマネージャーを取得
		EntityManager* GetEntityManager() { return m_pEntityManager.get(); }

	private:
		// システムリストのソート
		void sortSystem();
		// OnCreateコールバック関数
		static void CallOnCreate(System* pSystem);
	};

}
