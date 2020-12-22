//==================================================================
//												System.h
//	テンプレートシステムベース
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/23	テンプレートシステムクラス作成
//
//===================================================================

#pragma once

//====== インクルード部 ======
#include "ISystem.h"
#include <memory>

//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	template<class T>
	class System : public ISystem
	{
	public:
		// コンストラクタ
		explicit System(World* pWorld)
			: ISystem(pWorld) {}
		// デストラクタ
		virtual ~System() { m_ComponentList.clear(); }

		// リストに格納
		void AddList(T* com)
		{
			// 格納
			m_ComponentList.push_back(com);
		}
		// リストから削除
		void RemoveList(T* com)
		{
			// 検索
			auto itr = std::find(m_ComponentList.begin(), m_ComponentList.end(), com);
			// なかった
			if (m_ComponentList.end() == itr) return;
			// 削除
			m_ComponentList.erase(itr);
		}

	protected:
		std::list<T*> m_ComponentList;
	};
}
