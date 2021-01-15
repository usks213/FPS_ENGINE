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

//====== �C���N���[�h�� ======
#include "Animator2D.h"
#include "Animation2D.h"

#include "../ECS/Entity/EntityManager.h"
#include "../ECSEntity/GameObject.h"

// �V�X�e��
#include "../ECSSystem/Animation2DSystem.h"

// �����_���[
#include "Renderer.h"
#include "SpriteRenderer.h"
#include "MeshRenderer.h"
#include "BillboardRenderer.h"
#include "AssimpRenderer.h"



using namespace ECS;

//===== �}�N����` =====


//===== �N���X��` =====



//===== �v���g�^�C�v�錾 =====



//========================================
//
//	�R���X�g���N�^
//
//========================================
Animator2D::Animator2D()
{
	m_nCurrentAnimNo = 0;
	m_nFrame = 0;
	m_nSpeed = 0;
	m_nSplitX = 0;
	m_nSplitY = 0;
}


//========================================
//
//	�f�X�g���N�^
//
//========================================
Animator2D::~Animator2D()
{
	m_animationPool.clear();
}

//========================================
//
//	�X�V
//
//========================================
void Animator2D::OnCreate()
{
	// �V�X�e���Ɋi�[
	Animation2DSystem* sys = GetEntityManager()->GetWorld()->GetSystem<Animation2DSystem>();
	if (sys) sys->AddList(this);


	// �X�v���C�g
	const auto& spr = m_Parent.lock()->GetComponent<SpriteRenderer>();
	m_renderer = spr;
	if (!spr)
	{
		// ���b�V��
		const auto& mesh = m_Parent.lock()->GetComponent<MeshRenderer>();
		m_renderer = mesh;
		if (!mesh)
		{
			// �����_���[�擾
			const auto& bill = m_Parent.lock()->GetComponent<BillboardRenderer>();
			m_renderer = bill;
			if (!bill)
			{
				// �A�V���v
				const auto& assimp = m_Parent.lock()->GetComponent<AssimpRenderer>();
				m_renderer = assimp;
			}
		}
	}
}

//========================================
//
//	�j����
//
//========================================
void Animator2D::OnDestroy()
{
	// �V�X�e�����珜�O
	Animation2DSystem* sys = GetEntityManager()->GetWorld()->GetSystem<Animation2DSystem>();
	if (sys) sys->RemoveList(this);
}

//========================================
//
//	�X�V
//
//========================================
void Animator2D::AnimationUpdate()
{
	// ���݂̃A�j���[�V�������擾
	const auto& pAnimation = m_pCurrentAnimation.lock();

	// ��Ȃ��΂�
	if (!pAnimation) return;

	Vector3 texPos;
	Vector3 texSize = m_renderer.lock()->GetTexSize();

	// �A�j���[�V�����̃R�}�̍X�V
	if (0 == m_nFrame % m_nSpeed)
	{
		// �e�N�X�`�����W�̍X�V
		texPos->x = texSize->x * (pAnimation->GetAnimNoArray()[m_nCurrentAnimNo] % m_nSplitX);
		texPos->y = texSize->y * (pAnimation->GetAnimNoArray()[m_nCurrentAnimNo] / m_nSplitX);
		m_renderer.lock()->SetTexPos(texPos);
		m_renderer.lock()->UpdateTexMatrix();

		// ���݂̃R�}�ԍ��̍X�V
		m_nCurrentAnimNo++;
		m_nFrame = 0;
	}

	//  �t���[���̍X�V
	m_nFrame++;

	// �A�j���[�V�����̏I��(�ŏI�t���[��)
	if (m_nCurrentAnimNo >= pAnimation->GetArraySize() && m_nFrame >= m_nSpeed)
	{
		// �A�j���[�V�����I���̃R�[���o�b�N�֐�
		m_Parent.lock()->SendComponentMessage("OnAnimationExit2D", this);
		// ���Z�b�g
		m_nCurrentAnimNo = 0;
	}



}


//========================================
//
//	�A�j���[�^�[�ɃA�j���[�V�����̃Z�b�g
//
//========================================
void Animator2D::SetAnimation(std::string name)
{
	// �A�j���[�V�����̌���
	const auto& itr = m_animationPool.find(name);

	// ���݂�����
	if (m_animationPool.end() != itr)
	{
		const auto& curAnim = m_pCurrentAnimation.lock();
		// �A�j���[�V�������ς���ĂȂ��Ȃ�
		if (curAnim == itr->second) return;

		m_pCurrentAnimation = itr->second;
		m_nFrame = 0;
		m_nCurrentAnimNo = 0;
		return;
	}
}

//========================================
//
//	�A�j���[�^�[�ɃA�j���[�V�����̃Z�b�g
//
//========================================
void Animator2D::SetAnimation(const std::string name, int nSpeed)
{
	SetAnimation(name);
	if(nSpeed > 0)
		m_nSpeed = nSpeed;
}

//========================================
//
//	�A�j���[�^�[�ɃA�j���[�V�����̃Z�b�g
//
//========================================
void Animator2D::SetAnimation(const std::string name, int nSpeed, int nSplitX, int nSplitY)
{
	SetAnimation(name, nSpeed);
	m_nSplitX = nSplitX;
	m_nSplitY = nSplitY;

	Vector3 texSize;
	texSize->x = 1.0f / nSplitX;
	texSize->y = 1.0f / nSplitY;

	m_renderer.lock()->SetTexSize(texSize);
}


//========================================
//
//	�A�j���[�V�����̒ǉ�
//
//========================================
void Animator2D::AddAnimationSelect(const std::string name, int nMaxFrame, ...)
{
	// �ψ������X�g
	va_list args;
	int nCount;

	// ���I�ɃA�j���[�V�����N���X�̊m��
	Animation2D *pAnim = new Animation2D;

	// �ψ������X�g�쐬
	va_start(args, nMaxFrame);

	// �ψ����̐����v�Z
	nCount = nMaxFrame;

	// �ψ������������m��
	pAnim->m_pAnimNoArray = new int[nCount];

	// �z���nAnimNo�����ԂɊi�[
	for (int i = 0; i < nCount; i++)
	{
		pAnim->m_pAnimNoArray[i] = va_arg(args, int);
	}

	// �ψ������X�g�I��
	va_end(args);

	// �A�j���[�V�����̖��O�̊i�[
	pAnim->m_name = name;
	// �z��(�A�j���[�V�����̃R�})�̐����i�[
	pAnim->m_nArraySize = nCount;

	// �Ō�ɃA�j���[�^�[�̃��X�g�ɐV����������A�j���[�V�������i�[
	m_animationPool.emplace(name, pAnim);

}

//========================================
//
//	���� (�����ŃR�}�ԍ��U�蕪��)
//
//========================================
void Animator2D::AddAnimationDefault(const std::string name, int nMaxFrame)
{
	// ���I�ɃA�j���[�V�����N���X�̊m��
	Animation2D *pAnim = new Animation2D;

	// �������m��
	pAnim->m_pAnimNoArray = new int[nMaxFrame];

	// �z���nAnimNo�����ԂɊi�[
	for (int i = 0; i < (int)nMaxFrame; i++)
	{
		pAnim->m_pAnimNoArray[i] = i;
	}

	// �A�j���[�V�����̖��O�̊i�[
	pAnim->m_name = name;
	// �z��(�A�j���[�V�����̃R�})�̐����i�[
	pAnim->m_nArraySize = nMaxFrame;

	// �Ō�ɃA�j���[�^�[�̃��X�g�ɐV����������A�j���[�V�������i�[
	m_animationPool.emplace(name, pAnim);

}

//========================================
//
//	�e�N�X�`�������̎w��
//
//========================================
void Animator2D::SetSplit(int nSplitX, int nSplitY) {
	m_nSplitX = nSplitX; m_nSplitY = m_nSplitY;
	Vector3 texSize;
	texSize->x = 1.0f / nSplitX;
	texSize->y = 1.0f / nSplitY;
	m_renderer.lock()->SetTexSize(texSize);
}