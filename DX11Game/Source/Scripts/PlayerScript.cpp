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
#include "../Engine/Renderer/Light.h"

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
	transform().lock()->m_pos = Vector3(1000, 1000, 1000);
	transform().lock()->m_scale = Vector3(100, 200, 100);

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
	// ライト
	CLight::GetMainLight()->SetTargetPos(gameObject().lock()->transform().lock()->m_pos.GetFloat3());
}

//========================================
//
//	更新時
//
//========================================
void PlayerScript::Update()
{
	const float speed = 2.0f;
	const float jump = 14.0f;

	Vector3 forward = CCamera::GetMainCamera()->GetForward() * speed;
	forward->y = 0.0f;
	Vector3 right = forward.RotationY(-90);

	// 移動
	if (GetKeyPress(VK_W))
	{
		//transform().lock()->m_pos->z += 1.0f;
		m_rb.lock()->AddForce(forward);
	}
	if (GetKeyPress(VK_S))
	{
		//transform().lock()->m_pos->z -= 1.0f;
		m_rb.lock()->AddForce(forward * -1.0f);
	}
	if (GetKeyPress(VK_D))
	{
		//transform().lock()->m_pos->x += 1.0f;
		m_rb.lock()->AddForce(right);
	}
	if (GetKeyPress(VK_A))
	{
		//transform().lock()->m_pos->x -= 1.0f;
		m_rb.lock()->AddForce(right * -1.0f);
	}

	// ジャンプ
	if (GetKeyTrigger(VK_SPACE))
	{
		m_rb.lock()->SetForceY(jump);
	}


	// ショット
	if (GetMouseTrigger(MOUSEBUTTON_L))
	{
		const auto& test = GetEntityManager()->CreateEntity<GameObject>();
		test->AddComponent<MeshRenderer>()->MakeSphere("Bullet");
		const auto& rb = test->AddComponent<Rigidbody>();
		test->AddComponent<SphereCollider>();


		Vector3 dir = CCamera::GetMainCamera()->GetForward().normalized();

		test->transform().lock()->m_pos = transform().lock()->m_pos + dir * 200;
		test->transform().lock()->m_scale = Vector3{ 100, 100, 100 };

		rb->AddForce(dir * 100 + Vector3::WallVerticalVector(m_rb.lock()->GetForce(), dir));
		rb->SetDrag({ 0,0,0 });
		rb->SetGravityForce({ 0,0,0 });
		rb->SetStaticFriction(0);
		rb->SetDynamicFriction(0);
		rb->SetMass(10);
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

