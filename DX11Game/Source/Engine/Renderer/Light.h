#pragma once
#include "main.h"

class CLight
{
private:
	DirectX::XMFLOAT4 m_diffuse;
	DirectX::XMFLOAT4 m_ambient;
	DirectX::XMFLOAT4 m_specular;
	DirectX::XMFLOAT3 m_direction;

	XMFLOAT3 m_vPos;				// 視点
	XMFLOAT3 m_vTarget;				// 注視点
	XMFLOAT3 m_vUp;					// 上方ベクトル
	XMFLOAT4X4 m_mtxView;			// ビュー マトリックス

	float m_angle;

	XMFLOAT3 *m_pTargetPos;

	static CLight* m_mainLight;
public:
	CLight();

	void Init();
	void Update();
	DirectX::XMFLOAT4& GetDiffuse() { return m_diffuse; }
	DirectX::XMFLOAT4& GetAmbient() { return m_ambient; }
	DirectX::XMFLOAT4& GetSpecular() { return m_specular; }
	DirectX::XMFLOAT3& GetDir()		{ return m_direction; }
	DirectX::XMFLOAT4X4& GetViewMatrix() { return m_mtxView; }

	void SetTargetPos(XMFLOAT3 *pPos) { m_pTargetPos = pPos; }

	static void SetMainLight(CLight* pLight) { m_mainLight = pLight; }
	static CLight* GetMainLight() { return m_mainLight; }
};
