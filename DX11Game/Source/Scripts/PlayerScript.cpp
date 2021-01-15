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
#include "../Engine/System/debugproc.h"

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
#include "../Engine/ECSCompoent/DeltaCollider.h"

// レンダラー
#include "../Engine/Renderer/Camera.h"
#include "../Engine/Renderer/Light.h"
#include "../Engine/Renderer/PostProcessing.h"

#include "../Engine/ECSSystem/DeltaCollisionSystem.h"

// ワールド
#include "../Engine/ECS/World/WorldManager.h"
#include "../Engine/ECSWorld/TitleWorld.h"

// スクリプト
#include "BulletScript.h"
#include "DropDeltaScript.h"
#include "GameOverScript.h"


#include <iostream>


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
	// 名前・タグ
	gameObject().lock()->SetName("Player");
	gameObject().lock()->SetTag("Player");

	transform().lock()->m_pos = Vector3(0, 1000, 0);
	transform().lock()->m_scale = Vector3(300, 600, 300);

	// コンポーネントの追加

	// リジッドボディ
	const auto& rb = gameObject().lock()->AddComponent<Rigidbody>();
	m_rb = rb;
	rb->SetMass(2);

	//// レンダラー
	//const auto& renderer = gameObject().lock()->AddComponent<MeshRenderer>();
	//renderer->MakeSphere("Player", 100, 50);
	//renderer->SetDiffuseColor({ 0,1,0,1 });

	// コライダー
	//const auto& collider = gameObject().lock()->AddComponent<SphereCollider>();
	//collider->SetRadius(50);
	const auto& collider = gameObject().lock()->AddComponent<DeltaCollider>();

	// カメラ
	CCamera::GetMainCamera()->SetCameraTarget(gameObject().lock()->transform().lock());
	// ライト
	CLight::GetMainLight()->SetLightTarget(gameObject().lock()->transform().lock());


	// ジャンプ
	m_nJump = 0;
	// デルタカウンター
	m_nDeltaCount = 0;
	// ショット
	m_bShot = false;

	//HP
	m_fHP = m_fMaxHP;

	// ヒール
	m_fHeel = 5.0f / 60.0f;
	m_nHeelCnt = m_nHeelInteral;

	// ダメージ
	m_fDamage = 30.0f;
	m_nDamageCnt = m_nDamageInteral;

	// アクティブ
	m_bActive = true;
}

//========================================
//
//	更新時
//
//========================================
void PlayerScript::Update()
{
	// アクティブ
	if (!m_bActive) return;

	const float speed = 1.0f;
	const float jump = 20.0f;

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
	if (GetKeyTrigger(VK_SPACE) && 
		(transform().lock()->m_pos->y <= transform().lock()->m_scale->y / 2 || m_nJump > 0))
	{
		m_rb.lock()->SetForceY(jump + m_nJump);
		// サウンド
		CSound::PlaySE("Jump.wav", 0.7f);
		// 画面揺れ
		CCamera::GetMainCamera()->SetShakeFrame(8);
		m_bGround = false;
	}
	m_nJump--;
	if (m_nJump < 0) m_nJump = 0;

	// ショット
	m_nShotCnt--;
	if (m_bShot && m_nShotCnt < 0)
	{
		//const auto& test = GetEntityManager()->CreateEntity<GameObject>();
		const auto& test = Instantiate<GameObject>();
		test->AddComponent<BulletScript>();
		const auto& rb = test->GetComponent<Rigidbody>();

		Vector3 dir = CCamera::GetMainCamera()->GetForward().normalized();

		test->transform().lock()->m_pos = transform().lock()->m_pos + dir * 500;
		rb->AddForce(dir * 100 + Vector3::WallVerticalVector(m_rb.lock()->GetForce(), dir));
		rb->AddTorque(dir * 10);

		m_nShotCnt = 5;

		// サウンド
		CSound::PlaySE("Shot.wav", 0.12f);

	}
}

//========================================
//
//	後更新時
//
//========================================
void PlayerScript::LateUpdate()
{
	// アクティブ
	if (!m_bActive) return;

	// デバック表示
	PrintDebugProc("DeltaCount:%d\n", m_nDeltaCount);

	if (!m_bGround && transform().lock()->m_pos->y <= transform().lock()->m_scale->y / 2)
	{
		m_bGround = true;
		// 画面揺れ
		CCamera::GetMainCamera()->SetShakeFrame(6);
		// サウンド
		CSound::PlaySE("PlayerGround.wav", 1.0f);
	}


	// ステータス
	m_nHeelCnt--;
	if (m_nHeelCnt < 0)
	{
		// 回復
		m_fHP += m_fHeel;
		if (m_fHP > m_fMaxHP)
			m_fHP = m_fMaxHP;
	}

	// ダメージ
	m_nDamageCnt--;


	// HPを画面に反映
	auto post = PostProcessing::GetMain();
	post->GetColor()->y = m_fHP / m_fMaxHP;
	post->GetColor()->z = m_fHP / m_fMaxHP;
}

//========================================
//
//	終了時
//
//========================================
void PlayerScript::End()
{
	FILE* fp;
	int nScore = 0;

	// 前回のスコアを読み込む
	fopen_s(&fp, "data/score.bin", "rb");

	if (fp)
	{
		fread(&nScore, sizeof(int), 1, fp);

		fclose(fp);
	}

	if (m_nDeltaCount > nScore)
	{
		// スコアの書き出し
		fopen_s(&fp, "data/score.bin", "wb");

		if (fp)
		{
			fwrite(&m_nDeltaCount, sizeof(int), 1, fp);

			fclose(fp);
		}
	}
}


//******************** コールバック関数 ********************

//========================================
//
// 当たった時
//
//========================================
void PlayerScript::OnDeltaCollisionEnter(DeltaCollider* collider)
{
	// アクティブ
	if (!m_bActive) return;

	if (collider->gameObject().lock()->tag() == "DropDelta")
	{
		// カウンター加算
		m_nDeltaCount++;
	}
	else if (collider->gameObject().lock()->tag() == "BombCrystal")
	{
		m_nJump = 15;
		// 画面揺れ
		CCamera::GetMainCamera()->SetShakeFrame(6);
	}
	else if (collider->gameObject().lock()->tag() == "StartCrystal")
	{
		m_bShot = true;
		// 画面揺れ
		CCamera::GetMainCamera()->SetShakeFrame(6);
		// BGM
		CSound::PlayBGM("GameBGM.mp3", 0.3f);

	}
	else if (collider->gameObject().lock()->tag() == "Enemy")
	{
		// 当たった角度を計算
		Vector3 vec = collider->transform().lock()->m_pos - transform().lock()->m_pos;
		Vector3 forward = CCamera::GetMainCamera()->GetForward();

		if (Vector3::Dot(vec.normalized(), forward.normalized()) < -0.3f)
		{
			// ダメージ
			if (m_nDamageCnt > 0) return;
			m_nDamageCnt = m_nDamageInteral;

			// HP
			m_fHP -= m_fDamage;
			m_nHeelCnt = m_nHeelInteral;

			// 画面揺れ
			CCamera::GetMainCamera()->SetShakeFrame(16);

			// サウンド
			CSound::PlaySE("PlayerDamage.wav", 1.0f);

			if (m_fHP > 0) return;
			// ゲームオーバー
			const auto& gameover = GetEntityManager()->CreateEntity<GameObject>();
			gameover->AddComponent<GameOverScript>();

			// アクティブ
			m_bActive = false;

			// 削除
			Destroy(gameObject().lock());
		}
	}
}

//========================================
//
// 当たっている間
//
//========================================
void PlayerScript::OnDeltaCollisionStay(DeltaCollider* collider)
{
	// 同じ処理
	OnDeltaCollisionEnter(collider);
}

//========================================
//
// 離れた時
//
//========================================
void PlayerScript::OnDeltaCollisionExit(DeltaCollider* collider)
{

}

