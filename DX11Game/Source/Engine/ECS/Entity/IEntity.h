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
#include <list>


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	class IEntity : public Object
	{
		friend IComponent;
		using ComponentPool = std::list<std::shared_ptr<IComponent>>;

	public:
		// 自身のweakポインタ
		std::weak_ptr<IEntity> m_self;

	private:
		// コンポーネントプール
		ComponentPool m_ComponentPool;

	public:
		// コンストラクタ
		IEntity();
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
		virtual void Destroy() override;
		// メッセージ送信
		void SendComponentMessage(std::string message);

	protected:

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
		buffer->SetParent(m_self);
		// リストに格納
		m_ComponentPool.push_front(buffer);
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
				(*itr)->Destroy();
				m_ComponentPool.erase(itr);

				return true;
			}
		}
		return false;
	}
}
