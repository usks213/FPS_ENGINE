//==================================================================
//												IEntity.cpp
//	エンティティベース
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/21	ベースのエンティティクラス作成
//
//===================================================================


//===== インクルード部 =====
#include "IEntity.h"
#include "EntityManager.h"
#include <algorithm>

using namespace ECS;


//===== マクロ定義 =====


//===== プロトタイプ宣言 =====


//===== グローバル変数 =====


//===================================
//
//	コンストラクタ
//
//===================================
IEntity::IEntity(EntityManager* entityManager)
	: m_pEntityManager(entityManager)
{
}


//===================================
//
//	デストラクタ
//
//===================================
IEntity::~IEntity()
{
	// エンティティプールから除外
	m_pEntityManager->RemoveEntityPool(m_self);

	// コンポーネントクリア
	m_ComponentPool.clear();
}

//===================================
//
//	自身の破棄
//
//===================================
void IEntity::Destroy()
{
	// 自身消去
	m_pEntityManager->DestroyEntity(this);

	// コンポーネントの削除
	for (const auto& com : m_ComponentPool)
	{
		com->Destroy();
	}
}

//===================================
//
//	コンポーネントにメッセージ送信
//
//===================================
void IEntity::SendComponentMessage(std::string message)
{
	// メッセージ処理
	std::for_each(m_ComponentPool.begin(), m_ComponentPool.end(),
		[message](const auto& com)
		{
			com->SendMessage(message);
		});

	//for (const auto& com : m_ComponentPool)
	//{
	//	com->SendMessage(message);
	//}
}
