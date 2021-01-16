//==================================================================
//								FieldScript.cpp
//	フィールド
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/14	フィールドクリプトクラス作成
//
//===================================================================

//====== インクルード部 ======

// 自身
#include "FieldScript.h"

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
void FieldScript::Start()
{
	const auto& renderer = gameObject().lock()->AddComponent<MeshRenderer>();
	renderer->MakePlane("plane", 20, 20, 500, 500, 1, 1);
	//renderer->SetDiffuseTexture("data/texture/grid.png");
	renderer->SetNormalTexture("data/texture/sampledNormals.jpg");
	renderer->SetAmbientTexture("data/texture/skydome.png");
	m_renderer = renderer;
	//renderer->SetTexSize({ 100, 100, 0 });
	//renderer->UpdateTexMatrix();
	gameObject().lock()->transform().lock()->m_pos->y = -1;
}

//========================================
//
//	更新時
//
//========================================
void FieldScript::Update()
{
}

//========================================
//
//	後更新時
//
//========================================
void FieldScript::LateUpdate()
{
	const auto& target = m_target.lock();
	if (target)
	{
		transform().lock()->m_pos->x = target->transform().lock()->m_pos->x;
		transform().lock()->m_pos->z = target->transform().lock()->m_pos->z;
	}

	// テクスチャ座標の同期
	Vector3 pos = transform().lock()->m_pos;
	Vector3 texPos;
	texPos->x = pos->x / 500.0f;
	texPos->y = -pos->z / 500.0f;

	m_renderer.lock()->SetTexPos(texPos);
	m_renderer.lock()->UpdateTexMatrix();

}

//========================================
//
//	終了時
//
//========================================
void FieldScript::End()
{
}
