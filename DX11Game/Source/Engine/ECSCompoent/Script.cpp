//==================================================================
//								Script.cpp
//	�S�ẴX�N���v�g�R���|�[�l���g��
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/27	�X�N���v�g�x�[�X�N���X�쐬
//
//===================================================================


//====== �C���N���[�h�� ======
#include "Script.h"
#include "../ECSSystem/ScriptSystem.h"

#include "../ECS/Entity/EntityManager.h"
#include "Transform.h"


using namespace ECS;

//===== �}�N����` =====



//========================================
//
//	�R���X�g���N�^
//
//========================================
Script::Script()
{
}

//========================================
//
//	�f�X�g���N�^
//
//========================================
Script::~Script()
{
}

//========================================
//
//	������
//
//========================================
void Script::OnCreate()
{
	// �V�X�e���Ɋi�[
	ScriptSystem* sys = GetEntityManager()->GetWorld()->GetSystem<ScriptSystem>();
	if (sys) sys->AddList(this);


	// �g�����X�t�H�[���擾
	m_transform = m_Parent.lock()->GetComponent<Transform>();
	// �Q�[���I�u�W�F�N�g�擾
	m_gameObject = m_transform.lock()->gameObject();


	// �X�^�[�g�R�[���o�b�N
	this->Start();
}

//========================================
//
//	�j����
//
//========================================
void Script::OnDestroy()
{
	// �G���h�R�[���o�b�N
	this->End();

	// �V�X�e�����珜�O
	ScriptSystem* sys = GetEntityManager()->GetWorld()->GetSystem<ScriptSystem>();
	if (sys) sys->RemoveList(this);
}

//========================================
//
//	���b�Z�[�W��M
//
//========================================
void Script::SendComponentMessage(const std::string& message, void* value)
{
	// ���b�Z�[�W
	if		("Start" == message)		this->Start();
	else if ("Update" == message)		this->Update();
	else if ("LateUpdate" == message)	this->LateUpdate();
	else if ("End" == message)			this->End();

	// �����蔻��R�[���o�b�N
	else if ("OnCollisionEnter" == message)
	{
		this->OnCollisionEnter(static_cast<Collider*>(value));
	}
	else if ("OnCollisionStay" == message)
	{
		this->OnCollisionStay(static_cast<Collider*>(value));
	}
	else if ("OnCollisionExit" == message)
	{
		this->OnCollisionExit(static_cast<Collider*>(value));
	}

	// ECS�����蔻��R�[���o�b�N
	else if ("OnECSCollisionEnter" == message)
	{
		this->OnECSCollisionEnter(static_cast<SphereColliderData*>(value));
	}
	else if ("OnECSCollisionStay" == message)
	{
		this->OnECSCollisionStay(static_cast<SphereColliderData*>(value));
	}
	else if ("OnECSCollisionExit" == message)
	{
		this->OnECSCollisionExit(static_cast<SphereColliderData*>(value));
	}

	// �A�j���[�V�����R�[���o�b�N


}
