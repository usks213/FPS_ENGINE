//==================================================================
//												GameObject.h
//	ワールドで存在できるもの　ゲームオブジェクト
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/27	ゲームオブジェクトクラス作成
//
//===================================================================

#pragma once

//====== インクルード部 ======
#include "../ECS/Entity/IEntity.h"


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	// 前定義
	class Transform;

	class GameObject : public IEntity
	{
	public:
		// コンストラクタ
		explicit GameObject(EntityManager* entityManager);
		// デストラクタ
		virtual ~GameObject();
		
		// オブジェクト生成時
		void OnCreate() override;
		// オブジェクト破棄時
		void OnDestroy() override;

		// スタート
		virtual void Start() {};
		// エンド
		virtual void End() {};

		// 名前の設定
		void SetName(std::string name) { m_name = name; }
		// タグの設定
		void SetTag(std::string tag) { m_tag = tag; }
		// 名前の取得
		const std::string& name() { return m_name; }
		// タグの取得
		const std::string& tag()  { return m_tag; }

		// ゲームオブジェクト(自身)取得
		const std::weak_ptr<GameObject>& gameObject() { return m_gameObject; }
		// トランスフォーム取得
		const std::weak_ptr<Transform>& transform()   { return m_transform; }

		// アクティブ設定
		void SetActive(bool bActive) { m_bActive = bActive; }
		// アクティブ取得
		bool GetActive()			 { return m_bActive; }

	protected:
		// 名前
		std::string m_name;
		// タグ
		std::string m_tag;

		// ゲームオブジェクト(自身)
		std::weak_ptr<GameObject> m_gameObject;
		// トランスフォーム
		std::weak_ptr<Transform> m_transform;

		// アクティブ
		bool m_bActive;
	};
}
