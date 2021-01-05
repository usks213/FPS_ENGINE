//==================================================================
//												ECSRigidbodySystem.h
//	データ指向版リジッドボディ管理システム
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/01/05	データ指向版で作成
//
//===================================================================

#pragma once

//====== インクルード部 ======
#include "../ECS/System/ECSSystem.h"


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	// 前定義
	class RigidbodyData;

	class ECSRigidbodySystem final : public ECSSystem<RigidbodyData>
	{
	public:
		// コンストラクタ
		explicit ECSRigidbodySystem(World* pWorld);
		// デストラクタ
		virtual ~ECSRigidbodySystem();

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
