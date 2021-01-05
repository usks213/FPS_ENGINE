//==================================================================
//												IComponentData.h
//	データ指向用コンポーネントデータベース
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/05	コンポーネントデータベースクラス作成
//
//===================================================================

#pragma once

//====== インクルード部 ======
#include "../Object/IObject.h"
#include <string>


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	// 前定義
	class EntityManager;
	class IEntity;

	class IComponentData : public IObject
	{
		friend IEntity;
	public:
		// 親のエンティティ
		std::weak_ptr<IEntity> m_Parent;
		// 親のエンティティマネージャー
		EntityManager* m_pEntityManager;

	public:
		// コンストラクタ
		IComponentData();
		// デストラクタ
		virtual ~IComponentData();

		// 自身の破棄
		void Destroy() override;
		// メッセージを受信
		virtual void SendComponentMessage(const std::string& message, void* value = nullptr) {};

		// 親のエンティティの取得
		std::weak_ptr<IEntity> GetParent() { return m_Parent; }
		// 親のエンティティマネージャーを取得
		EntityManager* GetEntityManager() { return m_pEntityManager; }
	};

}
