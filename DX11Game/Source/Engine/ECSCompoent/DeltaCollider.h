//==================================================================
//								DeltaCollider.h
//	デルタ専用コライダー
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/09	デルタ専用コライダーベースクラス作成
//
//===================================================================

#pragma once

//====== インクルード部 ======
#include "../System/Math.h"
#include "../ECS/Component/IComponent.h"
#include <vector>
#include "Collider.h"

//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	// 前定義
	class DeltaCollisionSystem;
	class GameObject;
	class Transform;

	class DeltaCollider : public IComponent
	{
		friend DeltaCollisionSystem;

	public:
		// コンストラクタ
		DeltaCollider();
		// デストラクタ
		virtual ~DeltaCollider();

		// オブジェクト生成時
		void OnCreate() override;
		// オブジェクト破棄時
		void OnDestroy() override;

		// メッセージ受信
		void SendComponentMessage(const std::string& message, void* value) override;

		// 中心座標
		void SetCenter(Vector3 center) { m_bound.SetCenter(center); }
		// 当たり判定サイズ
		void SetRadius(float radius) { m_bound.SetSize(Vector3(2, 2, 2) * radius); }

		// メインコライダーか
		void SetMain(bool bMain) { m_bMain = bMain; }

		// ゲームオブジェクト取得
		const std::weak_ptr<GameObject>& gameObject() { return m_gameObject; }
		// トランスフォーム取得
		const std::weak_ptr<Transform>& transform() { return m_transform; }

	protected:
		// バウンドボリューム
		Bounds m_bound;
		// 状態
		bool m_bOldState;
		bool m_bCurState;
		// 形状
		bool m_bMain;

		// ゲームオブジェクト
		std::weak_ptr<GameObject> m_gameObject;
		// トランスフォーム
		std::weak_ptr<Transform> m_transform;
		
	public:
		// 球と球の当たり判定
		static bool SpheretoSphere(DeltaCollider* collider, DeltaCollider* other);
	};


}
