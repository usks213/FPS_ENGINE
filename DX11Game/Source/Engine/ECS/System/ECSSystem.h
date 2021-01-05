//==================================================================
//												ECSSystem.h
//	データ指向コンポーネントシステム
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/05	データ指向版作成
//
//===================================================================

#pragma once

//====== インクルード部 ======
#include "ISystem.h"
#include <memory>
#include <vector>


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	template<class T>
	class ECSSystem : public ISystem
	{
	public:
		// コンストラクタ
		explicit ECSSystem(World* pWorld)
			: ISystem(pWorld) {}
		// デストラクタ
		virtual ~ECSSystem() 
		{ 
			m_ComponentList.clear(); 
		}


		// コンポーネントデータ作成
		T* CreateComponentData()
		{
			T temp;
			auto itr = m_ComponentList.insert(m_ComponentList.end(), temp);
			return &*itr;
		}

		// コンポーネントデータ削除
		void DestroyComponentData(T* com)
		{
			// 検索
			auto itr = std::find_if(m_ComponentList.begin(), m_ComponentList.end(), 
				[&com](T& c)
				{
					return &c == com;
				});
			// なかった
			if (m_ComponentList.end() == itr) return;
			// 削除
			m_ComponentList.erase(itr);
		}

	protected:
		std::vector<T> m_ComponentList;
	};
}
