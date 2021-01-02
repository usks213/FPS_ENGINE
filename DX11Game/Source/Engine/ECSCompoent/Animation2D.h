//==================================================================
//									Animation2D.h
//	�A�j���[�V�����N���b�v
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/02	�A�j���[�V�����N���X�쐬
//
//===================================================================
#pragma once

//====== �C���N���[�h�� ======
#include <stdarg.h>
#include <string>


//===== �}�N����` =====


//===== �N���X��` =====

namespace ECS
{
	class Animator2D;

	class Animation2D final
	{
		friend Animator2D;
	public:
		Animation2D();
		~Animation2D();

		// �Q�b�g�֐�
		std::string GetName() { return m_name; }
		int* GetAnimNoArray() { return m_pAnimNoArray; }
		int GetArraySize() { return m_nArraySize; }
	private:
		int* m_pAnimNoArray;
		int m_nArraySize;
		std::string m_name;
	};
}