//==================================================================
//								ScriptBaseLayoutBaseLayout.cpp
//	スクリプトベースレイアウト
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/27	スクリプトベースレイアウトクラス作成
//
//===================================================================


//====== インクルード部 ======

// 自身
#include "ScriptBaseLayout.h"

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
#include "../Engine/ECSCompoent/BoxCollider.h"
#include "../Engine/ECSCompoent/SphereCollider.h"


// ネームスペース
using namespace ECS;


//===== マクロ定義 =====



//******************** スクリプトメソッド ********************


//========================================
//
//	開始時
//
//========================================
void ScriptBaseLayout::Start()
{
}

//========================================
//
//	更新時
//
//========================================
void ScriptBaseLayout::Update()
{
}

//========================================
//
//	後更新時
//
//========================================
void ScriptBaseLayout::LateUpdate()
{
}

//========================================
//
//	終了時
//
//========================================
void ScriptBaseLayout::End()
{
}


//******************** コールバック関数 ********************


//========================================
//
// 当たった時
//
//========================================
void ScriptBaseLayout::OnCollisionEnter(Collider* collider)
{

}

//========================================
//
// 当たっている間
//
//========================================
void ScriptBaseLayout::OnCollisionStay(Collider* collider)
{

}

//========================================
//
// 離れた時
//
//========================================
void ScriptBaseLayout::OnCollisionExit(Collider* collider)
{

}

