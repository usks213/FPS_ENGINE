//==================================================================
//								SkyDomeScript.h
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

#pragma once

//====== インクルード部 ======
#include "../Engine/ECSCompoent/Script.h"


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	class SkyDomeScript : public Script
	{
	private:
		// ----- メンバ -----
		std::weak_ptr<GameObject> m_target;

	public:
		// ターゲット
		void SetTarget(std::weak_ptr<GameObject> target) { m_target = target; }

	protected:
		// ----- メソッド -----

		// 開始時に呼ばれます
		void Start() override;
		// 毎フレーム呼ばれます
		void Update() override;
		// 毎フレーム更新後に呼ばれます
		void LateUpdate() override;
		// 終了時に呼ばれます
		void End() override;

	};
}
