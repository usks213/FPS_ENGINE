//==================================================================
//								TrackingMoveEnemyScript.cpp
//	追尾型エネミー
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/08	トラッキングムーブエネミークラス作成
//
//===================================================================


//====== インクルード部 ======

// 自身
#include "TrackingMoveEnemyScript.h"

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
#include "../Engine/ECSCompoent/BillboardRenderer.h"
#include "../Engine/ECSCompoent/AssimpRenderer.h"
#include "../Engine/ECSCompoent/SpriteRenderer.h"
#include "../Engine/ECSCompoent/InstancingMeshRenderer.h"
#include "../Engine/ECSCompoent/Rigidbody.h"
#include "../Engine/ECSCompoent/BoxCollider.h"
#include "../Engine/ECSCompoent/SphereCollider.h"

// ECSコンポーネント
#include "../Engine/ECSCompoent/DeltaCollider.h"

// ECSシステム
#include "../Engine/ECSSystem/DeltaCollisionSystem.h"


// ネームスペース
using namespace ECS;


//===== マクロ定義 =====



//******************** スクリプトメソッド ********************


//========================================
//
//	開始時
//
//========================================
void TrackingMoveEnemyScript::Start()
{
	// ベースクラス
	EnemyBaseScript::Start();

	// 名前・タグ
	gameObject().lock()->SetName("TrackingMoveEnemy");


	//--- コンポーネンの追加

	// インスタンシングレンダラー
	const auto& render = gameObject().lock()->AddComponent<InstancingMeshRenderer>();
	render->MakeDodecahedron("TrackingMoveEnemy");
	render->SetDiffuseColor({ 1,100 / 255.0f,0,1 });
}

//========================================
//
//	更新時
//
//========================================
void TrackingMoveEnemyScript::Update()
{
	// ベースクラス
	EnemyBaseScript::Update();
}

//========================================
//
//	後更新時
//
//========================================
void TrackingMoveEnemyScript::LateUpdate()
{
	// ベースクラス
	EnemyBaseScript::LateUpdate();
}

//========================================
//
//	終了時
//
//========================================
void TrackingMoveEnemyScript::End()
{
	// ベースクラス
	EnemyBaseScript::End();
}

