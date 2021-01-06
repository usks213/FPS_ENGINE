//==================================================================
//												ISystem.h
//	システムベース
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/22	システムクラス作成
//
//===================================================================

#pragma once

//====== インクルード部 ======
#include <list>
#include "../uniqueTypeID.h"

//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	// 前定義
	class World;
	class EntityManager;

	class ISystem 
	{
	public:
		// コンストラクタ
		explicit ISystem(World* pWorld);
		// デストラクタ
		virtual ~ISystem();

		// 生成時
		virtual void OnCreate();
		// 更新
		virtual void OnUpdate() {};
		// 後更新
		virtual void OnLateUpdate() {};
		// 描画
		virtual void OnDraw() {};
		// 削除時
		virtual void OnDestroy();

		// 更新処理の実行順を取得
		int GetUpdateOrder() { return m_nUpdateOrder; }

		// IDの登録
		template <class T>
		void SetTypeID() { m_nTypeID = CUniqueTypeID::Get<T>(); }
		// IDの取得
		int GetID() { return m_nTypeID; }

	protected:
		// エンティティマネージャーの取得
		EntityManager* GetEntityManager();

		// 更新処理の実行順を設定
		void SetUpdateOrder(int nOrder) { m_nUpdateOrder = nOrder; }

	private:
		// 親のワールド
		World* m_pWorld;
		// 更新処理の実行順
		int m_nUpdateOrder;

		// 型ID
		int m_nTypeID;
	};


	namespace UpdateOrder
	{
		enum EUpdateOrder
		{
			eScript = 0,
			eAnimation2D,
			eAnimation3D,
			eRigidbody,
			eCollision,
			eTransform,
			eRenderer,

			eMaxUpdateOrder,
		};
	}
}
