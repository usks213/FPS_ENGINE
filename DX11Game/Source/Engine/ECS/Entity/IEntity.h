//==================================================================
//												IEntity.h
//	エンティティベース
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/21	ベースのエンティティクラス作成
//
//===================================================================

#pragma once

//====== インクルード部 ======
#include "../Component/IComponent.h"
#include "../Object/ObjectManager.h"
#include <list>


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	// 前定義
	class EntityManager;

	class IEntity : public Object<IEntity>
	{
		friend IComponent;
		using ComponentPool = std::list<std::shared_ptr<IComponent>>;

	private:
		// コンポーネントプール
		ComponentPool m_ComponentPool;
		// 親のエンティティマネージャー
		EntityManager* m_pEntityManager;

	public:
		// コンストラクタ
		explicit IEntity(EntityManager* entityManager);
		// デストラクタ
		virtual ~IEntity();
		
		// コンポーネントの追加
		template<class T>
		std::shared_ptr<T> AddComponent();
		// コンポーネントの取得
		template<class T>
		std::shared_ptr<T> GetComponent();
		// コンポーネントの破棄
		template<class T>
		bool RemoveComponent();

		// 自身の破棄
		void Destroy() override;
		// メッセージ送信
		void SendComponentMessage(const std::string& message, void* value = nullptr);

		// 親のエンティティマネージャーを取得
		EntityManager* GetEntityManager() { return m_pEntityManager; }
	};

	// コンポーネント追加
	template <class T>
	std::shared_ptr<T> IEntity::AddComponent()
	{
		// コンポーネントの新規作成
		std::shared_ptr<T> buffer(new T());
		// コンポーネントIDを登録
		buffer->SetTypeID<T>();
		// 親オブジェクトを登録
		buffer->m_Parent = m_self;
		// 親のエンティティマネージャーを登録
		buffer->m_pEntityManager = m_pEntityManager;
		// リストに格納
		m_ComponentPool.push_front(buffer);
		// オブジェクトプールに格納
		ECS::ObjectManager::GetInstance()->AddObjectPool(buffer);
		// 初期化
		//buffer->Start();

		return buffer;
	}

	// コンポーネントを取得
	template <class T>
	std::shared_ptr<T> IEntity::GetComponent()
	{
		for (const auto& com : m_ComponentPool)
		{
			// コンポーネントIDを判定
			if (com->GetID() == CUniqueTypeID::Get<T>())
			{
				// 見つかったらキャスト
				auto buffer = std::static_pointer_cast<T>(com);

				return buffer;
			}
		}
		// なかったら
		return nullptr;
	}

	// コンポーネントを消去
	template <class T>
	bool IEntity::RemoveComponent()
	{
		for (auto itr = m_ComponentPool.begin(); itr != m_ComponentPool.end(); ++itr)
		{
			// コンポーネントIDを判定
			if ((*itr)->GetID() == CUniqueTypeID::Get<T>())
			{
				// コンポーネントの削除
				(*itr)->Destroy();

				return true;
			}
		}
		return false;
	}
}
