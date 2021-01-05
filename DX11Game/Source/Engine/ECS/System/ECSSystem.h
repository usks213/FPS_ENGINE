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
			auto itr = m_ComponentList.insert(m_ComponentList.end(), temp);
			return &*itr;
		}

		// �R���|�[�l���g�f�[�^�폜
		void DestroyComponentData(T* com)
		{
			// ����
			auto itr = std::find_if(m_ComponentList.begin(), m_ComponentList.end(), 
				[&com](T& c)
				{
					return &c == com;
				});
			// �Ȃ�����
			if (m_ComponentList.end() == itr) return;
			// �폜
			m_ComponentList.erase(itr);
		}

	protected:
		std::vector<T> m_ComponentList;
	};
}
