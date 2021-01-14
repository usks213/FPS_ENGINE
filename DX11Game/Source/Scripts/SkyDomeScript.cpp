//==================================================================
//								SkyDomeScript.cpp
//	スカイドームスクリプト
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/14	スカイドームスクリプトクラス作成
//
//===================================================================


//====== インクルード部 ======

// 自身
#include "SkyDomeScript.h"

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
void SkyDomeScript::Start()
{
}

//========================================
//
//	更新時
//
//========================================
void SkyDomeScript::Update()
{
}

//========================================
//
//	後更新時
//
//========================================
void SkyDomeScript::LateUpdate()
{
	const auto& target = m_target.lock();
	if (target)
	{
		transform().lock()->m_pos = target->transform().lock()->m_pos;
	}
}

//========================================
//
//	終了時
//
//========================================
void SkyDomeScript::End()
{
}
