//==================================================================
//								RigidbodyData.cpp
//	�f�[�^�w���Ń��W�b�h�{�f�B
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/05	�f�[�^�w���ō쐬
//
//===================================================================


//====== �C���N���[�h�� ======
#include "ECSRigidbody.h"
#include "../ECSSystem/ECSRigidbodySystem.h"

#include "../ECS/Entity/EntityManager.h"
#include "Transform.h"
#include "Collider.h"


using namespace ECS;

//===== �}�N����` =====
#define GRAVITY (9.8f / 60.0f)
#define DRAG (1.0f)

//#define MAX_VELOCITY (50.0f)


//========================================
//
//	�R���X�g���N�^
//
//========================================
RigidbodyData::RigidbodyData()
{
	// �ړ�
	m_velocity			= Vector3{0.0f, 0.0f, 0.0f};
	m_force				= Vector3{ 0.0f, 0.0f, 0.0f };
	m_drag				= Vector3{ 0.05f, 0.0f, 0.05f };
	// ��]
	m_angularVelocity	= Vector3{ 0.0f, 0.0f, 0.0f };
	m_torque			= Vector3{ 0.0f, 0.0f, 0.0f };
	m_torqueDrag		= Vector3{ 0.05f, 0.05f, 0.05f };
	// �����g�p
	m_bUsePhysics		= true;
	// �d��
	m_bUseGravity		= true;
	m_fGraviyForce		= Vector3{ 0.0f, -GRAVITY, 0.0f };
	// ����
	m_fMass				= 1.0f;
	// ���C
	m_fStaticFriction	= 0.0f;	// �Î~
	m_fDynamicFriction	= 0.0f;	// ��
	// �����W��
	m_e					= 1.0f;
}

//========================================
//
//	�f�X�g���N�^
//
//========================================
RigidbodyData::~RigidbodyData()
{

}

//========================================
//
//	������
//
//========================================
void RigidbodyData::OnCreate()
{
	// �g�����X�t�H�[���擾
	m_transform = m_Parent.lock()->GetComponent<Transform>();
	// �Q�[���I�u�W�F�N�g�擾
	m_gameObject = m_transform.lock()->gameObject();
}

//========================================
//
//	�j����
//
//========================================
void RigidbodyData::OnDestroy()
{
}

//========================================
//
//	���b�Z�[�W��M
//
//========================================
void RigidbodyData::SendComponentMessage(const std::string& message, void* value)
{
	// Update���b�Z�[�W
	if ("Update" == message)
	{
	}
}

//========================================
//
//	�X�V
//
//========================================
void RigidbodyData::Update()
{
	// �g�����X�t�H�[��
	const auto& pTrans = transform().lock();
	// ���W�E��]
	Vector3 pos = pTrans->m_pos;
	Vector3 rot = pTrans->m_rot;

	//===== ��] =====

	// �ړ���
	m_angularVelocity->x = 0;
	m_angularVelocity->y = 0;
	m_angularVelocity->z = 0;

	// �O��
	m_angularVelocity->x += m_torque->x;
	m_angularVelocity->y += m_torque->y;
	m_angularVelocity->z += m_torque->z;

	// ��]�̍X�V
	rot->x += m_angularVelocity->x;
	rot->y += m_angularVelocity->y;
	rot->z += m_angularVelocity->z;
	pTrans->m_rot = rot;

	// ��R�ŉ����x�����炷	fDrag(0.0f �` 1.0f)
	m_torque->x *= (1.0f - m_torqueDrag->x);
	if (fabsf(m_torque->x) < 0.01f) m_torque->x = 0.0f;
	m_torque->y *= (1.0f - m_torqueDrag->y);
	if (fabsf(m_torque->y) < 0.01f) m_torque->y = 0.0f;
	m_torque->z *= (1.0f - m_torqueDrag->z);
	if (fabsf(m_torque->z) < 0.01f) m_torque->z = 0.0f;


	//===== �ړ� =====

	// �ړ���
	m_velocity->x = 0;
	m_velocity->y = 0;
	m_velocity->z = 0;

	// �d��
	if (m_bUseGravity && m_bUsePhysics)
	{
		m_force += m_fGraviyForce * m_fMass;
	}

	// �O��
	m_velocity->x += m_force->x;
	m_velocity->y += m_force->y;
	m_velocity->z += m_force->z;

	// ���x���E
	//if (m_velocity->x > MAX_VELOCITY) m_velocity->x = MAX_VELOCITY;
	//if (m_velocity->y > MAX_VELOCITY) m_velocity->y = MAX_VELOCITY;
	//if (m_velocity->z > MAX_VELOCITY) m_velocity->z = MAX_VELOCITY;
	//if (m_velocity->x < -MAX_VELOCITY) m_velocity->x = -MAX_VELOCITY;
	//if (m_velocity->y < -MAX_VELOCITY) m_velocity->y = -MAX_VELOCITY;
	//if (m_velocity->z < -MAX_VELOCITY) m_velocity->z = -MAX_VELOCITY;

	// �ʒu�̍X�V
	pos->x += m_velocity->x;
	pos->y += m_velocity->y;
	pos->z += m_velocity->z;
	// �ړ����E
	if (pos->x < 0.0f)
	{
		pos->x = 0.0f;
		m_force->x = 0.0f;
	}
	if (pos->y < 0.0f)
	{
		pos->y = 0.0f;
		m_force->y = 0.0f;
	}
	if (pos->z < 0.0f)
	{
		pos->z = 0.0f;
		m_force->z = 0.0f;
	}
	// �X�V
	pTrans->m_pos = pos;
}

//========================================
//
//	��X�V
//
//========================================
void RigidbodyData::LateUpdate()
{
	// ������
	m_forceBackUp = Vector3{ 0,0,0 };

	// ��R�ŉ����x�����炷	fDrag(0.0f �` 1.0f)
	m_force->x *= (1.0f - m_drag->x);
	if (fabsf(m_force->x) < 0.01f) m_force->x = 0.0f;
	m_force->y *= (1.0f - m_drag->y);
	if (fabsf(m_force->y) < 0.01f) m_force->y = 0.0f;
	m_force->z *= (1.0f - m_drag->z);
	if (fabsf(m_force->z) < 0.01f) m_force->z = 0.0f;
}

// �����v�Z
void RigidbodyData::CollisionPhysics(const std::shared_ptr<RigidbodyData>& other, Vector3 normal)
{
	// ���W�b�h�{�f�B�擾
	const auto& rb1 = this;
	const auto& rb2 = other;
	if (!rb1 || !rb2) return;
	
	// �x�N�g���̑傫��
	float magnitude = rb1->m_velocity.magnitude();
	// �ǂ���x�N�g��
	Vector3 scratch = Vector3::WallScratchVector(rb1->m_velocity, normal);
	// �����x�N�g��
	Vector3 vertical = Vector3::WallVerticalVector(rb1->m_velocity, normal);
	Vector3 vertical2 = Vector3::WallVerticalVector(rb2->m_velocity, normal);


	//--- ��������(����)
	//float m1 = -rb2->m_fMass / (rb1->m_fMass + rb2->m_fMass);
	//float m2 =  rb1->m_fMass / (rb1->m_fMass + rb2->m_fMass);
	//Vector3 c = rb2->transform().lock()->m_pos - rb1->transform().lock()->m_pos;
	//float dot = Vector3::Dot((vertical - vertical2), c);
	float e = (rb1->m_e * rb2->m_e);

	//// ������̐������x
	//Vector3 verticalVector = vertical + c * dot * e * m1;
	//Vector3 verticalVector2 = vertical2 + c * dot * e * m2;

	// ������̐������x
	Vector3 verticalVector = 
		(vertical2 - vertical) * (1 + e) / (rb1->m_fMass / rb2->m_fMass + 1) + vertical;
	// ������̐������x(���葤)
	Vector3 verticalVector2 =
		(vertical - vertical2) * (1 + e) / (rb2->m_fMass / rb1->m_fMass + 1) + vertical2;


	//--- ��������(���C)
	// ���������̗�
	Vector3 horizontalVector;
	Vector3 horizontalVector2;
	// �����R��
	Vector3 N = vertical;
	// �Î~���C
	float myu_s = rb1->m_fStaticFriction * rb2->m_fStaticFriction;
	// �����C
	float myu_d = rb1->m_fDynamicFriction * rb2->m_fDynamicFriction;
	// �����C��
	float F_d = myu_d * N.magnitude();
	if (F_d > 1.0f) F_d = 1.0f;

	// �ő�Î~���C�͂��傫����
	if (scratch.magnitude() > myu_s * N.magnitude())
	{
		// ���������̗�
		horizontalVector = scratch - scratch * F_d;
	}
	// �ő�Î~���C�͂��傫����(���葤)
	if (F_d > myu_s * vertical2.magnitude())
	{
		// ���������̗�
		horizontalVector2 = scratch * F_d;
	}


	//--- �͂̍���
	// ���������邩
	if (rb1->m_bUsePhysics)
	{
		//--- �x�N�g���̍���
		Vector3 force = horizontalVector + verticalVector;

		// �ŏI�I�ȗ�
		//if (force.magnitude() > rb1->m_forceBackUp.magnitude())
		{
			rb1->m_force = force;
			rb1->m_forceBackUp = force;
			rb1->m_velocity = force;
		}
	}
	// ���������邩(���葤)
	if (rb2->m_bUsePhysics)
	{
		//--- �x�N�g���̍���
		Vector3 force = horizontalVector2 + verticalVector2;

		// �ŏI�I�ȗ�
		//if (force.magnitude() > rb2->m_forceBackUp.magnitude())
		{
			rb2->m_force = force;
			rb2->m_forceBackUp = force;
			rb2->m_velocity = force;
		}
	}
}
