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
	class IEntity;

	class IComponent : public Object<IComponent>
	{
	protected:
		// 親のエンティティ
		std::weak_ptr<IEntity> m_Parent;

	private:
		// 型ID
		int m_nTypeID;

	public:
		// コンストラクタ
		IComponent();
		// デストラクタ
		virtual ~IComponent();

		// メッセージを受信
		virtual void SendMessage(std::string message) {};

		// IDの登録
		template <class T>
		void SetTypeID() { m_nTypeID = CUniqueTypeID::Get<T>(); }
		// IDの取得
		int GetID() { return m_nTypeID; }

		// 親エンティティのセット
		void SetParent(std::weak_ptr<IEntity> entity) { m_Parent = entity; };
		// 親のエンティティの取得
		std::weak_ptr<IEntity> GetParent() { return m_Parent; }
	};

}
