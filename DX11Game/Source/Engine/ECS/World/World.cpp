//==================================================================
//												World.cpp
//	ワールドのベース
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/22	ワールドクラス作成
//
//===================================================================


//===== インクルード部 =====
#include "World.h"
#include "../Entity/EntityManager.h"
#include "../System/ISystem.h"
#include <algorithm>

// システム
#include "../../ECSSystem/TransformSystem.h"
#include "../../ECSSystem/RendererSystem.h"
#include "../../ECSSystem/Animation2DSystem.h"
#include "../../ECSSystem/CollisionSystem.h"
#include "../../ECSSystem/DeltaCollisionSystem.h"
#include "../../ECSSystem/RigidbodySystem.h"
#include "../../ECSSystem/ScriptSystem.h"

using namespace ECS;


//===== マクロ定義 =====


//===== プロトタイプ宣言 =====


//===== グローバル変数 =====


//===================================
//
//	コンストラクタ
//
//===================================
World::World(std::string name)
	:m_name(name)
{
	// エンティティマネージャーの生成
	m_pEntityManager.reset(new EntityManager(this));

	// システムの追加
	AddSystem<TransformSystem>();
	AddSystem<RendererSystem>();
	AddSystem<Animation2DSystem>();
	AddSystem<CollisionSystem>();
	AddSystem<DeltaCollisionSystem>();
	AddSystem<RigidbodySystem>();
	AddSystem<ScriptSystem>();
}


//===================================
//
//	デストラクタ
//
//===================================
World::~World()
{
	// エンティティの破棄
	GetEntityManager()->ClearEntityPool();

	// オブジェクトのプールの破棄
	ObjectManager::GetInstance()->ClearnUpObject();

	// システムリストの終了処理
	for (auto&& system : m_SystemList)
	{
		// OnDestroyを呼び出す
		system->OnDestroy();
	}
}


//===================================
//
//	システムリストの更新
//
//===================================
void World::Update()
{
	// システムリストの更新処理
	for (auto&& system : m_SystemList)
	{
		// OnUpdateを呼び出す
		system->OnUpdate();
	}
}

//===================================
//
//	システムリストの後更新
//
//===================================
void World::LateUpdate()
{
	// システムリストの更新処理
	for (auto&& system : m_SystemList)
	{
		// OnUpdateを呼び出す
		system->OnLateUpdate();
	}
}

//===================================
//
//	システムリストの描画
//
//===================================
void World::Draw()
{
	// システムリストの更新処理
	for (auto&& system : m_SystemList)
	{
		// OnUpdateを呼び出す
		system->OnDraw();
	}
}


//===================================
//
//	システムリストのソート
//
//===================================
void World::sortSystem()
{
	std::sort(m_SystemList.begin(), m_SystemList.end(),
		[](const std::unique_ptr<ISystem>& lhs, const std::unique_ptr<ISystem>& rhs)
		{
			// 昇順
			return lhs->GetUpdateOrder() < rhs->GetUpdateOrder();
		});
}

//= ==================================
//
//	システムOnCreateのコールバック
//
//===================================
void World::CallOnCreate(ISystem* pSystem)
{
	// OnCreateを呼び出す
	pSystem->OnCreate();
}

//= ==================================
//
//	終了時
//
//===================================
void World::OnDestroy()
{
	// エンティティの破棄
	GetEntityManager()->ClearEntityPool();

	// オブジェクトのプールの破棄
	ObjectManager::GetInstance()->ClearnUpObject();

	// システムリストの終了処理
	for (auto&& system : m_SystemList)
	{
		// OnDestroyを呼び出す
		system->OnDestroy();
	}
}