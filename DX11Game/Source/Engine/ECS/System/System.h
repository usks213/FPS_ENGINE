//==================================================================
//												System.h
//	�e���v���[�g�V�X�e���x�[�X
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/23	�e���v���[�g�V�X�e���N���X�쐬
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include "ISystem.h"
#include <memory>
#include <unordered_map>

//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	template<class T>
	class System : public ISystem
	{
	public:
		// �R���X�g���N�^
		explicit System(World* pWorld)
			: ISystem(pWorld) {}
		// �f�X�g���N�^
		virtual ~System() { m_ComponentList.clear(); }

		// ���X�g�Ɋi�[
		void AddList(T* com)
		{
			// �i�[
			m_ComponentList.push_back(com);

			ComItr itr = m_ComponentList.end();
			m_ComponentPool.emplace(com, --itr);
		}
		// ���X�g����폜
		void RemoveList(T* com)
		{
			//// ����
			//auto itr = std::find(m_ComponentList.begin(), m_ComponentList.end(), com);
			//// �Ȃ�����
			//if (m_ComponentList.end() == itr) return;
			//// �폜
			//m_ComponentList.erase(itr);

			// ����
			PoolItr itr = m_ComponentPool.find(com);
			// �Ȃ�����
			if (m_ComponentPool.end() == itr) return;

			// ���X�g����폜
			m_ComponentList.erase(itr->second);
			// �v�[������폜
			m_ComponentPool.erase(itr);
		}

		// �R���|�[�l���g�̃\�[�g
		void SortComponentList() override
		{
			m_ComponentList.sort([](T* left, T* right)
				{
					return left < right;
				});
		}

	protected:
		std::list<T*> m_ComponentList;
		using ComItr = typename std::list<T*>::iterator;
		std::unordered_map<const T*, ComItr> m_ComponentPool;
		using PoolItr = typename std::unordered_map<const T*, ComItr>::iterator;
	};
}
