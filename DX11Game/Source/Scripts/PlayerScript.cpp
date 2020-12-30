//==================================================================
//								PlayerScript.cpp
//	プレイヤースクリプト
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/27	プレイヤースクリプトクラス作成(Test)
//
//===================================================================


//====== インクルード部 ======

// 自身
#include "PlayerScript.h"

// システム
#include "../Engine/System/input.h"
#include "../Engine/System/Sound.h"

// マネージャー
#include "../Engine/ECS/Entity/EntityManager.h"

// エンティティ
#include "../Engine/ECSEntity/GameObject.h"

// コンポーネント
#include "../Engine/ECSCompoent/Transform.h"
#include "../Engine/ECSCompoent/MeshRenderer.h"
#include "../Engine/ECSCompoent/Rigidbody.h"
#include "../Engine/ECSCompoent/BoxCollider.h"
#include "../Engine/ECSCompoent/SphereCollider.h"

#include "../Engine/Renderer/Camera.h"

// ネームスペース
using namespace ECS;


//===== マクロ定義 =====



//******************** スクリプトメソッド ********************


//========================================
//
//	開始時
//
//========================================
void PlayerScript::Start()
{
	transform().lock()->m_pos = Vector3(100, 400, 200);
	transform().lock()->m_scale = Vector3(100, 100, 100);

	// コンポーネントの追加

	// リジッドボディ
	const auto& rb = gameObject().lock()->AddComponent<Rigidbody>();
	m_rb = rb;
	rb->SetMass(2);

	// レンダラー
	const auto& renderer = gameObject().lock()->AddComponent<MeshRenderer>();
	//renderer->MakeSphere("Player", 100, 50);
	renderer->MakeCube("Player");
	renderer->SetDiffuseColor({ 0,1,0,1 });

	// コライダー
	//const auto& collider = gameObject().lock()->AddComponent<SphereCollider>();
	//collider->SetRadius(50);
	const auto& collider = gameObject().lock()->AddComponent<BoxCollider>();

	// カメラ
	CCamera::GetMainCamera()->SetCameraTarget(gameObject().lock()->transform().lock());
}

//========================================
//
//	更新時
//
//========================================
void PlayerScript::Update()
{
	if (GetKeyPress(VK_UP))
	{
		//transform().lock()->m_pos->z += 1.0f;
		m_rb.lock()->AddForceZ(2.0f);
	}
	if (GetKeyPress(VK_DOWN))
	{
		//transform().lock()->m_pos->z -= 1.0f;
		m_rb.lock()->AddForceZ(-2.0f);
	}
	if (GetKeyPress(VK_RIGHT))
	{
		//transform().lock()->m_pos->x += 1.0f;
		m_rb.lock()->AddForceX(2.0f);
	}
	if (GetKeyPress(VK_LEFT))
	{
		//transform().lock()->m_pos->x -= 1.0f;
		m_rb.lock()->AddForceX(-2.0f);
	}

	if (GetKeyTrigger(VK_SPACE))
	{
		m_rb.lock()->SetForceY(8.0f);
	}
}

//========================================
//
//	後更新時
//
//========================================
void PlayerScript::LateUpdate()
{
}

//========================================
//
//	終了時
//
//========================================
void PlayerScript::End()
{
}


//******************** コールバック関数 ********************


//========================================
//
// 当たった時
//
//========================================
void PlayerScript::OnCollisionEnter(Collider* collider)
{
	// 削除
	//GetEntityManager()->DestroyEntity(collider->gameObject().lock());
}

//========================================
//
// 当たっている間
//
//========================================
void PlayerScript::OnCollisionStay(Collider* collider)
{
	// 色変更
	collider->gameObject().lock()->GetComponent<MeshRenderer>()
		->SetDiffuseColor({ 1,0,0,1 });
}

//========================================
//
// 離れた時
//
//========================================
void PlayerScript::OnCollisionExit(Collider* collider)
{
	// 色変更
	collider->gameObject().lock()->GetComponent<MeshRenderer>()
		->SetDiffuseColor({ 1,1,1,1 });
}

