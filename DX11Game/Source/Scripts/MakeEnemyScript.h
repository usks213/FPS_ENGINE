//==================================================================
//								MakeEnemyScript.h
//	エネミー生成スクリプト
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/08	エネミー生成スクリプトクラス作成
//
//===================================================================

#pragma once

//====== インクルード部 ======
#include "../Engine/ECSCompoent/Script.h"


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	class MakeEnemyScript : public Script
	{
	private:
		// ----- メンバ -----
		enum EEnemyType
		{
			eStraightCircle,
			eTracking,
			eGroup,

			eMaxType,
		};

		// プレイヤーオブジェクト
		std::weak_ptr<GameObject> m_player;

		// 生成タイミング
		int m_nSpawnInterval;
		// 生成カウント
		int m_nSpawnCount;
		// 生成数
		int m_nSpawnNum;
		// タイプ
		int m_nType;

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

	public:
		// プレイヤーのセット
		void SetPlayer(std::weak_ptr<GameObject> player) { m_player = player; }
	};
}
