//==================================================================
//												ScriptSystem.h
//	スクリプト管理システム
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/27	スクリプトシステム作成
//
//===================================================================

#pragma once

//====== インクルード部 ======
#include "../ECS/System/System.h"


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	// 前定義
	class Script;

	class ScriptSystem final : public System<Script>
	{
	public:
		// コンストラクタ
		explicit ScriptSystem(World* pWorld);
		// デストラクタ
		virtual ~ScriptSystem();

		// 生成時
		void OnCreate() override;
		// 更新
		void OnUpdate() override;
		// 後更新
		void OnLateUpdate() override;
		// 削除時
		void OnDestroy() override;
	};
}
