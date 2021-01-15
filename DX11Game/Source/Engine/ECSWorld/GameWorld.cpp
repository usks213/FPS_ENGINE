//==================================================================
//												GameWorld.cpp
//	ゲームワールド
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/11	ゲームワールドクラス作成
//				ゲームシーンを移植
//
//===================================================================


//===== インクルード部 =====
#include "GameWorld.h"

// システム
#include "../ECS/Entity/EntityManager.h"
#include "../ECSEntity/GameObject.h"
#include "../Renderer/Camera.h"
#include "../System/Sound.h"

// コンポーネント
#include "../ECSCompoent/Transform.h"
#include "../ECSCompoent/MeshRenderer.h"

// スクリプト
#include "../../Scripts/PlayerScript.h"
#include "../../Scripts/MakeEnemyScript.h"
#include "../../Scripts/StartCrystalScript.h"
#include "../../Scripts/SkyDomeScript.h"
#include "../../Scripts/DeltaUIScript.h"


using namespace ECS;


//===== マクロ定義 =====


//===== プロトタイプ宣言 =====


//===== グローバル変数 =====


//===================================
//
//	コンストラクタ
//
//===================================
GameWorld::GameWorld(std::string name)
	:World(name)
{
}

//===================================
//
//	デストラクタ
//
//===================================
GameWorld::~GameWorld()
{
}

//===================================
//
//	ワールドの初期化
//
//===================================
void GameWorld::Start()
{
	//--- ゲームオブジェクトを追加

	// プレイヤー
	const auto& player = GetEntityManager()->CreateEntity<GameObject>();
	const auto & playerScript = player->AddComponent<PlayerScript>();

	// デルタUI
	const auto& deltaUi = GetEntityManager()->CreateEntity<GameObject>();
	deltaUi->AddComponent<DeltaUIScript>()->SetPlayer(playerScript);

	//// エネミーメイカー
	//const auto& enemyMaker = GetEntityManager()->CreateEntity<GameObject>();
	//enemyMaker->AddComponent<MakeEnemyScript>()->SetPlayer(player);

	// スタートクリスタル
	const auto& crystal = GetEntityManager()->CreateEntity<GameObject>();
	CCamera::GetMainCamera()->Update();
	Vector3 dir = CCamera::GetMainCamera()->GetForward();
	dir *= 1500;
	dir->y = 700;
	crystal->transform().lock()->m_pos = dir;
	crystal->AddComponent<StartCrystalScript>()->SetPlayer(player);

	// スカイドーム
	const auto& sky = GetEntityManager()->CreateEntity<GameObject>();
	const auto& skyRn = sky->AddComponent<MeshRenderer>();
	skyRn->MakeSkyDome("Sky", 100);
	skyRn->SetDiffuseTexture("data/texture/sky02.png");
	skyRn->SetLighting(false);
	skyRn->SetLayer(VIEW_FAR_Z);
	skyRn->SetUpdateLayer(false);
	//skyRn->SetRendererBlendState(BS_ALPHABLEND);
	//sky->transform().lock()->m_scale = Vector3{ VIEW_FAR_Z * 1.5f, VIEW_FAR_Z * 1.5f, VIEW_FAR_Z * 1.5f };
	sky->transform().lock()->m_scale = Vector3{ FOG_FAR_Z * 2.5f, FOG_FAR_Z * 2.5f, FOG_FAR_Z * 2.5f };
	sky->AddComponent<SkyDomeScript>()->SetTarget(player);

	// 床
	const auto& plane = GetEntityManager()->CreateEntity<GameObject>();
	const auto& renderer = plane->AddComponent<MeshRenderer>();
	renderer->MakePlane("plane", 1000, 1000, 500, 500, 1, 1);
	//renderer->SetDiffuseTexture("data/texture/grid.png");
	renderer->SetNormalTexture("data/texture/sampledNormals.jpg");
	renderer->SetAmbientTexture("data/texture/sky02.png");
	//renderer->SetTexSize({ 100, 100, 0 });
	//renderer->UpdateTexMatrix();
	plane->transform().lock()->m_pos->y = -1;
	

}

//===================================
//
//	ワールドの終了
//
//===================================
void GameWorld::End()
{
	// BGM
	CSound::StopBGM("GameBGM.mp3");

}
