//==================================================================
//									Animator2D.h
//	�A�j���[�^�[2D
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/02	�A�j���[�^�[2D�N���X�쐬
//
//===================================================================
#pragma once

//====== �C���N���[�h�� ======
#include "../ECS/Component/IComponent.h"
#include <map>


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	// �O��`
	class Animation2D;
	class Renderer;

	// �����F �ėp�^�ɂ���
	class Animator2D final : public IComponent
	{
	public:
		// �R���X�g���N�^
		Animator2D();
		// �f�X�g���N�^
		~Animator2D();

		// �I�u�W�F�N�g������
		void OnCreate() override;
		// �I�u�W�F�N�g�j����
		void OnDestroy() override;

		// �A�j���[�V�����A�b�v�f�[�g
		void AnimationUpdate();

		// �A�j���[�V�������X�g�Ǘ�
		void AddAnimationSelect(const std::string name, int nMaxFrame, ...);
		void AddAnimationDefault(const std::string name, int nMaxFrame);
		void SetAnimation(const std::string name);
		void SetAnimation(const std::string name, int nSpeed);
		void SetAnimation(const std::string name, int nSpeed, int nSplitX, int nSplitY);

		// �Z�b�g�֐�
		void SetSplit(int nSplitX, int nSplitY);
		void SetAnimationSpeed(int nSpeed) { m_nSpeed = nSpeed; }

		// �Q�b�g�֐�
		std::weak_ptr<Animation2D> GetCurrentAnimation() { return m_pCurrentAnimation; }

	private:
		// �v�[��
		std::map<const std::string, std::shared_ptr<Animation2D>> m_animationPool;

		// �����_���[
		std::weak_ptr<Renderer> m_renderer;

		// �A�j���[�V�����̕ϐ�
		int m_nSpeed;
		int m_nSplitX;
		int m_nSplitY;
		int m_nFrame;
		int m_nCurrentAnimNo;
		std::weak_ptr<Animation2D> m_pCurrentAnimation;
	};
}