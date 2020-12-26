//==================================================================
//								Collider.h
//	コライダーベース
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/24	コライダーベースクラス作成
//
//===================================================================

#pragma once

//====== インクルード部 ======
#include "../System/Math.h"
#include "../ECS/Component/IComponent.h"
#include "Transform.h"

//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	class Bounds
	{
	private:
		Vector3 m_center;
		Vector3 m_size;
	public:
		// コンストラクタ
		Bounds(Vector3 center, Vector3 size);

		// ゲット関数
		Vector3 GetCenter()		{ return m_center; }
		Vector3 GetHalfSize()	{ return m_size * 0.5f; }
		Vector3 GetMax()		{ return m_center + GetHalfSize(); }
		Vector3 GetMin()		{ return m_center - GetHalfSize(); }
		Vector3 GetSize()		{ return m_size; }
		float   GetRadius()		{ return GetHalfSize()->x; }

		// セット関数
		void SetCenter(Vector3 center)	{ m_center = center; }
		void SetSize(Vector3 size)		{ m_size = size; }
	};

	// 前定義
	class CollisionSystem;

	class Collider : public IComponent
	{
		friend CollisionSystem;
	public:
		enum EColliderType
		{
			eBox,
			eSphere,

			eMaxColliderType,
		};

	public:
		// コンストラクタ
		Collider();
		// デストラクタ
		virtual ~Collider();

		// オブジェクト生成時
		void OnCreate() override;
		// オブジェクト破棄時
		void OnDestroy() override;

		// メッセージ受信
		void SendComponentMessage(const std::string& message, void* value) override;

		// 各コライダーの当たり判定
		virtual bool Judgment(Collider* other) = 0;
		// コライダータイプ
		virtual EColliderType GetColliderType() = 0;

		// 中心座標
		void SetCenter(Vector3 center) { m_bound.SetCenter(center); }

	protected:
		// バウンドボリューム
		Bounds m_bound;
		// トリガー
		bool m_bTriggr;
		// トランスフォーム
		std::weak_ptr<Transform> m_trans;
		// 状態
		bool m_bOldState;
		bool m_bCurState;
		
	//=== 各当たり判定 ===
	public:
		// AABBtoAABB 仮判定用
		static bool AABBtoAABB(Collider* collider, Collider* other);
	protected:
		// 詳細判定　押し出しあり
		static bool BoxToBox(Collider* collider, Collider* other);
		static bool BoxToSphere(Collider* collider, Collider* other);
		static bool SphereToBox(Collider* collider, Collider* other);
		static bool SphereToSphere(Collider* collider, Collider* other);
	};


}
