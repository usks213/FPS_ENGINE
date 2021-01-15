//==================================================================
//								Script.cpp
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


//====== インクルード部 ======
#include "Script.h"
#include "../ECSSystem/ScriptSystem.h"

#include "../ECS/Entity/EntityManager.h"
#include "Transform.h"


using namespace ECS;

//===== マクロ定義 =====



//========================================
//
//	コンストラクタ
//
//========================================
Script::Script()
{
}

//========================================
//
//	デストラクタ
//
//========================================
Script::~Script()
{
}

//========================================
//
//	生成時
//
//========================================
void Script::OnCreate()
{
	// システムに格納
	ScriptSystem* sys = GetEntityManager()->GetWorld()->GetSystem<ScriptSystem>();
	if (sys) sys->AddList(this);


	// トランスフォーム取得
	m_transform = m_Parent.lock()->GetComponent<Transform>();
	// ゲームオブジェクト取得
	m_gameObject = m_transform.lock()->gameObject();


	// スタートコールバック
	this->Start();
}

//========================================
//
//	破棄時
//
//========================================
void Script::OnDestroy()
{
	// エンドコールバック
	this->End();

	// システムから除外
	ScriptSystem* sys = GetEntityManager()->GetWorld()->GetSystem<ScriptSystem>();
	if (sys) sys->RemoveList(this);
}

//========================================
//
//	メッセージ受信
//
//========================================
void Script::SendComponentMessage(const std::string& message, void* value)
{
	// メッセージ
	if		("Start" == message)		this->Start();
	else if ("Update" == message)		this->Update();
	else if ("LateUpdate" == message)	this->LateUpdate();
	else if ("End" == message)			this->End();

	// 当たり判定コールバック
	else if ("OnCollisionEnter" == message)
	{
		this->OnCollisionEnter(static_cast<Collider*>(value));
	}
	else if ("OnCollisionStay" == message)
	{
		this->OnCollisionStay(static_cast<Collider*>(value));
	}
	else if ("OnCollisionExit" == message)
	{
		this->OnCollisionExit(static_cast<Collider*>(value));
	}

	// Delta当たり判定コールバック
	else if ("OnDeltaCollisionEnter" == message)
	{
		this->OnDeltaCollisionEnter(static_cast<DeltaCollider*>(value));
	}
	else if ("OnDeltaCollisionStay" == message)
	{
		this->OnDeltaCollisionStay(static_cast<DeltaCollider*>(value));
	}
	else if ("OnDeltaCollisionExit" == message)
	{
		this->OnDeltaCollisionExit(static_cast<DeltaCollider*>(value));
	}

	// アニメーションコールバック
	else if ("OnAnimationExit2D" == message)
	{
		this->OnAnimationExit2D(static_cast<Animator2D*>(value));
	}

}
