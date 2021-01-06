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

#include "../Component/IComponentData.h"


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
			m_ComponentList.push_back(temp);
			m_bReLoad = true;
			return &m_ComponentList.back();
		}

		// コンポーネントデータ削除
		void DestroyComponentData(T* com)
		{
			for (int i = 0; i < m_ComponentList.size(); ++i)
			{
				if (&m_ComponentList[i] == com)
				{
					m_ComponentList[i] = m_ComponentList.back();
					m_ComponentList.pop_back();
					// ポインタ付け替え
					m_ComponentList[i].m_pHost->m_data = &m_ComponentList[i];
					break;
				}
			}
		}

		// コンポーネンデータの再登録処理
		void ReLoadComponentData()
		{
			if (!m_bReLoad) return;

			for (auto& com : m_ComponentList)
			{
				com.m_pHost->m_data = &com;
			}
			m_bReLoad = false;
		}

	protected:
		std::vector<T> m_ComponentList;
		bool m_bReLoad;
	};
}
