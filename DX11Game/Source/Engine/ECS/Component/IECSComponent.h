//==================================================================
//												IECSComponent.h
//	データ指向用コンポーネントベース
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/05	データ指向版コンポーネントベースクラス作成
//
//===================================================================

#pragma once

//====== インクルード部 ======
#include "IComponent.h"
#include "../Entity/EntityManager.h"

//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	template<class DATA, class SYSTEM>
	class IECSComponent : public IComponent
	{
	public:
		// オブジェクト生成時
		void OnCreate() override
		{
			// システムからデータを追加作成
			m_data = GetEntityManager()->GetWorld()->
				GetSystem<SYSTEM>()->CreateComponentData();

			// 親オブジェクトを登録
			m_data->m_Parent = m_Parent;
			// 親のエンティティマネージャーを登録
			m_data->m_pEntityManager = m_pEntityManager;
			// 生成後関数
			m_data->OnCreate();
		}

		// オブジェクト破棄時
		void OnDestroy() override
		{
			// システムからデータを削除
			GetEntityManager()->GetWorld()->
				GetSystem<SYSTEM>()->DestroyComponentData(m_data);
		}

		// メッセージ受信
		void SendComponentMessage(const std::string& message, void* value) override
		{
			// データ内にメッセージ送信
			m_data->SendComponentMessage(message, value);
		}

		// データ取得
		DATA* GetData() { return m_data; }

		// 演算子
		DATA* operator -> ()
		{
			return GetData();
		}

	private:
		// コンポーネントデータ
		DATA* m_data;
	};
}
