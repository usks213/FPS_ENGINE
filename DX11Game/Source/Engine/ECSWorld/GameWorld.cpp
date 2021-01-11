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

// コンポーネント
#include "../ECSCompoent/Transform.h"
#include "../ECSCompoent/MeshRenderer.h"

// スクリプト
#include "../../Scripts/PlayerScript.h"
#include "../../Scripts/MakeEnemyScript.h"

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
	player->AddComponent<PlayerScript>();

	// エネミーメイカー
	const auto& enemyMaker = GetEntityManager()->CreateEntity<GameObject>();
	enemyMaker->AddComponent<MakeEnemyScript>()->SetPlayer(player);

	// 床
	const auto& test = GetEntityManager()->CreateEntity<GameObject>();
	const auto renderer = test->AddComponent<MeshRenderer>();
	renderer->MakeCube("plane");
	renderer->SetDiffuseTexture("data/texture/grid.png");
	renderer->SetTexSize({ 100, 100, 0 });
	renderer->UpdateTexMatrix();
	test->transform().lock()->m_scale = Vector3{ 100000, 1, 100000 };

}

//===================================
//
//	ワールドの終了
//
//===================================
void GameWorld::End()
{
}
