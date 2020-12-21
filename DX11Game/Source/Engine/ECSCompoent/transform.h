//==================================================================
//								transform.h
//	トランスフォーム
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/07/22	トランスフォームクラスの作成
//	2020/09/23	コンポーネント化
//
//===================================================================

#pragma once

//====== インクルード部 ======
#include "CComponent.h"

//===== マクロ定義 =====


//===== クラス定義 =====
class CTransform : public CComponent
{
public:
	CTransform();
	~CTransform();

	void LateUpdate() override;

	// ゲット関数
	Vector3 GetRotation() { return m_rot; }
	Vector3 GetPos() { return m_pos; }
	Vector3 GetScale() { return m_scale; }
	Vector3 GetDir() { return m_dir; }
	XMFLOAT4X4* GetWorldMatrix() { return &m_mtxWorld; }
	// セット関数
	void SetPos(Vector3 pos) { m_pos = pos; }
	void SetRotation(Vector3 rot) { m_rot = rot; }
	void SetScale(Vector3 scale) { m_scale = scale; }
	void SetDir(Vector3 dir) { m_dir = dir; }

	Vector3 m_pos;				// 中心座標
	Vector3 m_scale;				// 縦横サイズ
	Vector3 m_rot;				// オブジェクトの回転角度
	Vector3 m_dir;				// 向き
	XMFLOAT4X4 m_mtxWorld;

private:

};

