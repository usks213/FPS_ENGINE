//==================================================================
//												IComponent.h
//	コンポーネントベース
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/21	コンポーネントベースクラス作成
//
//===================================================================

#pragma once

//====== インクルード部 ======
#include "../Object/Object.h"
#include "../uniqueTypeID.h"
#include <string>


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	// 前定義
	class EntityManager;
	class IEntity;

	class IComponent : public Object<IComponent>
	{
		friend IEntity;
	protected:
		// 親のエンティティ
		std::weak_ptr<IEntity> m_Parent;
		// 親のエンティティマネージャー
		EntityManager* m_pEntityManager;
	private:
		// 型ID
		int m_nTypeID;

	public:
		// コンストラクタ
		IComponent();
		// デストラクタ
		virtual ~IComponent();

		// 自身の破棄
		void Destroy() override;
		// メッセージを受信
		virtual void SendComponentMessage(const std::string& message, void* value = nullptr) {};

		// IDの登録
		template <class T>
		void SetTypeID() { m_nTypeID = CUniqueTypeID::Get<T>(); }
		// IDの取得
		int GetID() { return m_nTypeID; }

		// 親のエンティティの取得
		std::weak_ptr<IEntity> GetParent() { return m_Parent; }
		// 親のエンティティマネージャーを取得
		EntityManager* GetEntityManager() { return m_pEntityManager; }
	};

}
