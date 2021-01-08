#pragma once

#include <DirectXMath.h>
#include <cmath>

using namespace DirectX;

class Vector3
{
public:

	Vector3() {
		m_vec = { 0.0f, 0.0f, 0.0f };
	}
	Vector3(const Vector3& vec) {
		m_vec = vec.m_vec;
	}
	Vector3(XMFLOAT3 vec) {
		m_vec = vec;
	}
	Vector3(float x, float y, float z) {
		m_vec.x = x;
		m_vec.y = y;
		m_vec.z = z;
	}
	~Vector3() {}

	void operator = (Vector3 vec)
	{
		m_vec = vec.m_vec;
	}
	void operator = (XMFLOAT3 vec)
	{
		m_vec = vec;
	}

	void operator += (Vector3 vec)
	{
		m_vec.x += vec.m_vec.x;
		m_vec.y += vec.m_vec.y;
		m_vec.z += vec.m_vec.z;
	}
	void operator -= (Vector3 vec)
	{
		m_vec.x -= vec.m_vec.x;
		m_vec.y -= vec.m_vec.y;
		m_vec.z -= vec.m_vec.z;
	}
	void operator *= (Vector3 vec)
	{
		m_vec.x *= vec.m_vec.x;
		m_vec.y *= vec.m_vec.y;
		m_vec.z *= vec.m_vec.z;
	}
	void operator /= (Vector3 vec)
	{
		m_vec.x /= vec.m_vec.x;
		m_vec.y /= vec.m_vec.y;
		m_vec.z /= vec.m_vec.z;
	}

	void operator *= (float vec)
	{
		m_vec.x *= vec;
		m_vec.y *= vec;
		m_vec.z *= vec;
	}
	void operator /= (float vec)
	{
		m_vec.x /= vec;
		m_vec.y /= vec;
		m_vec.z /= vec;
	}

	Vector3 operator + (Vector3 vec)
	{
		Vector3 v = *this;
		v += vec;
		return v;
	}
	Vector3 operator - (Vector3 vec)
	{
		Vector3 v = *this;
		v -= vec;
		return v;
	}
	Vector3 operator * (Vector3 vec)
	{
		Vector3 v = *this;
		v *= vec;
		return v;
	}
	Vector3 operator / (Vector3 vec)
	{
		Vector3 v = *this;
		v /= vec;
		return v;
	}

	Vector3 operator * (float vec)
	{
		Vector3 v = *this;
		v *= vec;
		return v;
	}
	Vector3 operator / (float vec)
	{
		Vector3 v = *this;
		v /= vec;
		return v;
	}

	XMFLOAT3* operator -> ()
	{
		return &m_vec;
	}


	// ����
	static float Dot(Vector3 vec1, Vector3 vec2)
	{
		float dot;

		dot = vec1->x * vec2->x + vec1->y * vec2->y + vec1->z * vec2->z;

		return dot;
	}

	// �O��
	static Vector3 Cross(Vector3 vec1, Vector3 vec2)
	{
		Vector3 vec;
		vec->x = vec1->y*vec2->z - vec1->z*vec2->y;
		vec->y = vec1->z*vec2->x - vec1->x*vec2->z;
		vec->z = vec1->x*vec2->y - vec1->y*vec2->x;
		return vec;
	}

	// �O�_�O��
	static Vector3 Cross(Vector3 center, Vector3 pos1, Vector3 pos2)
	{
		return Cross(pos1 - center, pos2 - center);
	}

	// �O��2D
	static float Cross2D(Vector3 vec1, Vector3 vec2)
	{
		return vec1->x * vec2->y - vec1->y * vec2->x;
	}

	// �x�N�g���̒���
	static float Length(Vector3 vec)
	{
		return sqrtf(vec->x * vec->x + vec->y * vec->y + vec->z * vec->z);
	}

	// ��_�Ԃ̋���
	static float Length(Vector3 vec1, Vector3 vec2)
	{
		Vector3 vec = vec1 - vec2;
		return sqrtf(vec->x * vec->x + vec->y * vec->y + vec->z * vec->z);
	}

	// �x�N�g���̐��K��
	static Vector3 Normalize(Vector3 vec)
	{
		float mag = Length(vec);
		if (!mag) return vec;
		vec->x /= mag;
		vec->y /= mag;
		vec->z /= mag;

		return vec;
	}

	// �x�N�g���̑傫��
	float magnitude()
	{
		Vector3 vec = *this;
		return sqrtf(vec->x * vec->x + vec->y * vec->y + vec->z * vec->z);
	}
	// �x�N�g���̑傫�� �������Ȃ�
	float magnitudeNoSqrt()
	{
		Vector3 vec = *this;
		return vec->x * vec->x + vec->y * vec->y + vec->z * vec->z;
	}

	// �x�N�g���̐��K��
	Vector3 normalized()
	{
		Vector3 vec = *this;
		float mag = Length(vec);
		if (!mag) return vec;
		vec->x /= mag;
		vec->y /= mag;
		vec->z /= mag;

		return vec;
	}

	// �ǂ���x�N�g��
	static Vector3 WallScratchVector(Vector3 front, Vector3 normal)
	{
		normal = normal.normalized();
		return (front - normal * Dot(front, normal));
	}

	// ���˃x�N�g��
	static Vector3 WallReflectVector(Vector3 front, Vector3 normal)
	{
		normal = normal.normalized();
		return (front - normal * Dot(front, normal) * 2.0f);
	}

	// �����x�N�g��
	static Vector3 WallVerticalVector(Vector3 front, Vector3 normal)
	{
		normal = normal.normalized();
		return normal * Dot(front, normal);
	}

	// X����]
	Vector3 RotationX(float angle)
	{
		float rad = XMConvertToRadians(angle);
		XMFLOAT3 v = m_vec;

		v.y = m_vec.y * cosf(rad) + m_vec.z * sinf(rad);
		v.z= -m_vec.y * sinf(rad) + m_vec.z * cosf(rad);

		return v;
	}

	// Y����]
	Vector3 RotationY(float angle)
	{
		float rad = XMConvertToRadians(angle);
		XMFLOAT3 v = m_vec;

		v.x =  m_vec.x * cosf(rad) - m_vec.z * sinf(rad);
		v.z =  m_vec.x * sinf(rad) + m_vec.z * cosf(rad);

		return v;
	}

	// Z����]
	Vector3 RotationZ(float angle)
	{
		float rad = XMConvertToRadians(angle);
		XMFLOAT3 v = m_vec;

		v.x =  m_vec.x * cosf(rad) + m_vec.y * sinf(rad);
		v.y = -m_vec.x * sinf(rad) + m_vec.y * cosf(rad);

		return v;
	}


	// �����擾
	XMFLOAT3* GetFloat3() { return &m_vec; }

private:
	XMFLOAT3 m_vec;
};
