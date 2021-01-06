//==================================================================
//												ECSSystem.h
//	�f�[�^�w���R���|�[�l���g�V�X�e��
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/05	�f�[�^�w���ō쐬
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include "ISystem.h"
#include <memory>
#include <vector>

#include "../Component/IComponentData.h"


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	template<class T>
	class ECSSystem : public ISystem
	{
	public:
		// �R���X�g���N�^
		explicit ECSSystem(World* pWorld)
			: ISystem(pWorld) {}
		// �f�X�g���N�^
		virtual ~ECSSystem() 
		{ 
			m_ComponentList.clear(); 
		}


		// �R���|�[�l���g�f�[�^�쐬
		T* CreateComponentData()
		{
			T temp;
			m_ComponentList.push_back(temp);
			m_bReLoad = true;
			return &m_ComponentList.back();
		}

		// �R���|�[�l���g�f�[�^�폜
		void DestroyComponentData(T* com)
		{
			for (int i = 0; i < m_ComponentList.size(); ++i)
			{
				if (&m_ComponentList[i] == com)
				{
					m_ComponentList[i] = m_ComponentList.back();
					m_ComponentList.pop_back();
					// �|�C���^�t���ւ�
					m_ComponentList[i].m_pHost->m_data = &m_ComponentList[i];
					break;
				}
			}
		}

		// �R���|�[�l���f�[�^�̍ēo�^����
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
