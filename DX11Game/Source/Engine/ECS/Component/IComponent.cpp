//==================================================================
//												IComponent.cpp
//	コンポーネントベース
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/21	コンポーネントベースクラス作成
//
//===================================================================


//===== インクルード部 =====
#include "IComponent.h"
#include "../Entity/IEntity.h"

using namespace ECS;


//===== マクロ定義 =====


//===== プロトタイプ宣言 =====


//===== グローバル変数 =====


//===================================
//
//	コンストラクタ
//
//===================================
IComponent::IComponent()
	: m_nTypeID(-1)
{
}


//===================================
//
//	デストラクタ
//
//===================================
IComponent::~IComponent()
{
	// 親のエンティティを取得
	const auto& entity = m_Parent.lock();
	if (!entity) return;

	// 検索
	auto itr = std::find(entity->m_ComponentPool.begin(), entity->m_ComponentPool.end(), m_self.lock());

	// なかったら
	if (entity->m_ComponentPool.end() == itr) return;

	// 親のコンポーネントリストから削除
	entity->m_ComponentPool.erase(itr);

}




