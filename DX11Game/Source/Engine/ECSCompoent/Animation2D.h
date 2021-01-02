//==================================================================
//									Animation2D.h
//	アニメーションクリップ
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/02	アニメーションクラス作成
//
//===================================================================
#pragma once

//====== インクルード部 ======
#include <stdarg.h>
#include <string>


//===== マクロ定義 =====


//===== クラス定義 =====

namespace ECS
{
	class Animator2D;

	class Animation2D final
	{
		friend Animator2D;
	public:
		Animation2D();
		~Animation2D();

		// ゲット関数
		std::string GetName() { return m_name; }
		int* GetAnimNoArray() { return m_pAnimNoArray; }
		int GetArraySize() { return m_nArraySize; }
	private:
		int* m_pAnimNoArray;
		int m_nArraySize;
		std::string m_name;
	};
}