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
#include "../ECS/Entity/EntityManager.h"

//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	// 前定義
	class ScriptSystem;
	class GameObject;
	class Transform;
	class Collider;
	class DeltaCollider;
	class DeltaCollider;
	class Rigidbody;
	class Animator2D;

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

	public:
		//--- ゲームオブジェクト関連

		// ゲームオブジェクト生成
		template<class T> 
		std::shared_ptr<T> Instantiate(Vector3 pos = { 0,0,0 }, Vector3 rot = { 0,0,0 }, Vector3 scale = { 0,0,0 });

		// ゲームオブジェクトの破棄
		void Destroy(const std::shared_ptr<IEntity>& obj) { Destroy(obj.get()); }
		void Destroy(IEntity* obj) { GetEntityManager()->DestroyEntity(obj); }

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

		// Delta当たり判定
		virtual void OnDeltaCollisionEnter(DeltaCollider* collider) {};
		virtual void OnDeltaCollisionStay(DeltaCollider* collider) {};
		virtual void OnDeltaCollisionExit(DeltaCollider* collider) {};

		// アニメーション
		virtual void OnAnimationExit2D(Animator2D* animator) {};

	private:
		// ゲームオブジェクト
		std::weak_ptr<GameObject> m_gameObject;
		// トランスフォーム
		std::weak_ptr<Transform> m_transform;
	};


	// ゲームオブジェクト生成
	template<class T>
	std::shared_ptr<T> Script::Instantiate(Vector3 pos, Vector3 rot, Vector3 scale)
	{
		// ゲームオブジェクト生成
		const auto& obj = GetEntityManager()->CreateEntity<T>();
		// トランスフォーム取得
		const std::shared_ptr<Transform>& trans = obj->GetComponent<Transform>();

		// 値を代入
		trans->m_pos = pos;
		trans->m_rot = rot;
		trans->m_scale = scale;

		return obj;
	}
}
