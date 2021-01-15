//==================================================================
//												TitleWorld.cpp
//	タイトルワールド
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/15	タイトルワールドクラス作成
//
//===================================================================


//===== インクルード部 =====
#include "TitleWorld.h"

// システム
#include "../ECS/Entity/EntityManager.h"
#include "../ECSEntity/GameObject.h"
#include "../Renderer/Camera.h"
#include "../System/Sound.h"
#include "../System/Input.h"
#include "../Renderer/PostProcessing.h"

// コンポーネント
#include "../ECSCompoent/Transform.h"
#include "../ECSCompoent/SpriteRenderer.h"

// スクリプト
#include "../../Scripts/TitleScoreUIScript.h"
#include "../../Scripts/TitleButtonScript.h"


using namespace ECS;


//===== マクロ定義 =====


//===== プロトタイプ宣言 =====


//===== グローバル変数 =====


//===================================
//
//	コンストラクタ
//
//===================================
TitleWorld::TitleWorld(std::string name)
	:World(name)
{
}

//===================================
//
//	デストラクタ
//
//===================================
TitleWorld::~TitleWorld()
{
}

//===================================
//
//	ワールドの初期化
//
//===================================
void TitleWorld::Start()
{
	// ポストエフェクト
	PostProcessing::GetMain()->GetColor()->y = 1.0f;
	PostProcessing::GetMain()->GetColor()->z = 1.0f;

	// タイトル
	const auto& title = GetEntityManager()->CreateEntity<GameObject>();
	const auto& titleSpr = title->AddComponent<SpriteRenderer>();
	titleSpr->SetDiffuseTexture("data/texture/FadeDelta.png");
	titleSpr->SetLayer(titleSpr->eBG1);
	title->transform().lock()->m_pos->y = SCREEN_CENTER_Y / 6;
	title->transform().lock()->m_scale = Vector3{ SCREEN_WIDTH, SCREEN_HEIGHT, 1 };

	// スコア
	const auto& score = GetEntityManager()->CreateEntity<GameObject>();
	score->AddComponent<TitleScoreUIScript>();

	// タイトルボタン
	const auto& button = GetEntityManager()->CreateEntity<GameObject>();
	button->AddComponent<TitleButtonScript>();
	button->transform().lock()->m_pos->y = -SCREEN_CENTER_Y / 3;

	// カーソルの表示
	SetShowCursor(true);
}

//===================================
//
//	ワールドの終了
//
//===================================
void TitleWorld::End()
{
}
