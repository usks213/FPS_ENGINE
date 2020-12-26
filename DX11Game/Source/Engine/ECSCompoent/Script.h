//==================================================================
//								Script.h
//	全てのスクリプトコンポーネント元
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/27	スクリプトベースクラス作成
//
//===================================================================

#pragma once

//====== インクルード部 ======
#include "../System/Math.h"
#include "../ECS/Component/IComponent.h"


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	// 前定義
	class ScriptSystem;
	class GameObject;
	class Transform;
	class Collider;


	class Script : public IComponent
	{
		friend ScriptSystem;
	public:
		// コンストラクタ
		Script();
		// デストラクタ
		virtual ~Script();

		// オブジェクト生成時
		void OnCreate() override;
		// オブジェクト破棄時
		void OnDestroy() override;

		// メッセージ受信
		void SendComponentMessage(const std::string& message, void* value = nullptr) override;

		// ゲームオブジェクト取得
		const std::weak_ptr<GameObject>& gameObject() { return m_gameObject; }
		// トランスフォーム取得
		const std::weak_ptr<Transform>& transform() { return m_transform; }

	protected:
		// スクリプト関数
		virtual void Start(){}
		virtual void Update(){}
		virtual void LateUpdate(){}
		virtual void End(){}

		//--- コールバック関数

		// 当たり判定
		virtual void OnCollisionEnter(Collider* collider) {};
		virtual void OnCollisionStay(Collider* collider)  {};
		virtual void OnCollisionExit(Collider* collider)  {};

		// アニメーション


	private:
		// ゲームオブジェクト
		std::weak_ptr<GameObject> m_gameObject;
		// トランスフォーム
		std::weak_ptr<Transform> m_transform;
	};
}
