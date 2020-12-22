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
		}
		// ���X�g����폜
		void RemoveList(T* com)
		{
			// ����
			auto itr = std::find(m_ComponentList.begin(), m_ComponentList.end(), com);
			// �Ȃ�����
			if (m_ComponentList.end() == itr) return;
			// �폜
			m_ComponentList.erase(itr);
		}

	protected:
		std::list<T*> m_ComponentList;
	};
}
