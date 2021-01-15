//==================================================================
//									Animator2D.h
//	アニメーター2D
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/02	アニメーター2Dクラス作成
//
//===================================================================

//====== インクルード部 ======
#include "Animator2D.h"
#include "Animation2D.h"

#include "../ECS/Entity/EntityManager.h"
#include "../ECSEntity/GameObject.h"

// システム
#include "../ECSSystem/Animation2DSystem.h"

// レンダラー
#include "Renderer.h"
#include "SpriteRenderer.h"
#include "MeshRenderer.h"
#include "BillboardRenderer.h"
#include "AssimpRenderer.h"



using namespace ECS;

//===== マクロ定義 =====


//===== クラス定義 =====



//===== プロトタイプ宣言 =====



//========================================
//
//	コンストラクタ
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
//	デストラクタ
//
//========================================
Animator2D::~Animator2D()
{
	m_animationPool.clear();
}

//========================================
//
//	更新
//
//========================================
void Animator2D::OnCreate()
{
	// システムに格納
	Animation2DSystem* sys = GetEntityManager()->GetWorld()->GetSystem<Animation2DSystem>();
	if (sys) sys->AddList(this);


	// スプライト
	const auto& spr = m_Parent.lock()->GetComponent<SpriteRenderer>();
	m_renderer = spr;
	if (!spr)
	{
		// メッシュ
		const auto& mesh = m_Parent.lock()->GetComponent<MeshRenderer>();
		m_renderer = mesh;
		if (!mesh)
		{
			// レンダラー取得
			const auto& bill = m_Parent.lock()->GetComponent<BillboardRenderer>();
			m_renderer = bill;
			if (!bill)
			{
				// アシンプ
				const auto& assimp = m_Parent.lock()->GetComponent<AssimpRenderer>();
				m_renderer = assimp;
			}
		}
	}
}

//========================================
//
//	破棄時
//
//========================================
void Animator2D::OnDestroy()
{
	// システムから除外
	Animation2DSystem* sys = GetEntityManager()->GetWorld()->GetSystem<Animation2DSystem>();
	if (sys) sys->RemoveList(this);
}

//========================================
//
//	更新
//
//========================================
void Animator2D::AnimationUpdate()
{
	// 現在のアニメーションを取得
	const auto& pAnimation = m_pCurrentAnimation.lock();

	// 空なら飛ばす
	if (!pAnimation) return;

	Vector3 texPos;
	Vector3 texSize = m_renderer.lock()->GetTexSize();

	// アニメーションのコマの更新
	if (0 == m_nFrame % m_nSpeed)
	{
		// テクスチャ座標の更新
		texPos->x = texSize->x * (pAnimation->GetAnimNoArray()[m_nCurrentAnimNo] % m_nSplitX);
		texPos->y = texSize->y * (pAnimation->GetAnimNoArray()[m_nCurrentAnimNo] / m_nSplitX);
		m_renderer.lock()->SetTexPos(texPos);
		m_renderer.lock()->UpdateTexMatrix();

		// 現在のコマ番号の更新
		m_nCurrentAnimNo++;
		m_nFrame = 0;
	}

	//  フレームの更新
	m_nFrame++;

	// アニメーションの終了(最終フレーム)
	if (m_nCurrentAnimNo >= pAnimation->GetArraySize() && m_nFrame >= m_nSpeed)
	{
		// アニメーション終了のコールバック関数
		m_Parent.lock()->SendComponentMessage("OnAnimationExit2D", this);
		// リセット
		m_nCurrentAnimNo = 0;
	}



}


//========================================
//
//	アニメーターにアニメーションのセット
//
//========================================
void Animator2D::SetAnimation(std::string name)
{
	// アニメーションの検索
	const auto& itr = m_animationPool.find(name);

	// 存在したら
	if (m_animationPool.end() != itr)
	{
		const auto& curAnim = m_pCurrentAnimation.lock();
		// アニメーションが変わってないなら
		if (curAnim == itr->second) return;

		m_pCurrentAnimation = itr->second;
		m_nFrame = 0;
		m_nCurrentAnimNo = 0;
		return;
	}
}

//========================================
//
//	アニメーターにアニメーションのセット
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
//	アニメーターにアニメーションのセット
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
//	アニメーションの追加
//
//========================================
void Animator2D::AddAnimationSelect(const std::string name, int nMaxFrame, ...)
{
	// 可変引数リスト
	va_list args;
	int nCount;

	// 動的にアニメーションクラスの確保
	Animation2D *pAnim = new Animation2D;

	// 可変引数リスト作成
	va_start(args, nMaxFrame);

	// 可変引数の数を計算
	nCount = nMaxFrame;

	// 可変引数分メモリ確保
	pAnim->m_pAnimNoArray = new int[nCount];

	// 配列にnAnimNoを順番に格納
	for (int i = 0; i < nCount; i++)
	{
		pAnim->m_pAnimNoArray[i] = va_arg(args, int);
	}

	// 可変引数リスト終了
	va_end(args);

	// アニメーションの名前の格納
	pAnim->m_name = name;
	// 配列(アニメーションのコマ)の数を格納
	pAnim->m_nArraySize = nCount;

	// 最後にアニメーターのリストに新しく作ったアニメーションを格納
	m_animationPool.emplace(name, pAnim);

}

//========================================
//
//	生成 (自動でコマ番号振り分け)
//
//========================================
void Animator2D::AddAnimationDefault(const std::string name, int nMaxFrame)
{
	// 動的にアニメーションクラスの確保
	Animation2D *pAnim = new Animation2D;

	// メモリ確保
	pAnim->m_pAnimNoArray = new int[nMaxFrame];

	// 配列にnAnimNoを順番に格納
	for (int i = 0; i < (int)nMaxFrame; i++)
	{
		pAnim->m_pAnimNoArray[i] = i;
	}

	// アニメーションの名前の格納
	pAnim->m_name = name;
	// 配列(アニメーションのコマ)の数を格納
	pAnim->m_nArraySize = nMaxFrame;

	// 最後にアニメーターのリストに新しく作ったアニメーションを格納
	m_animationPool.emplace(name, pAnim);

}

//========================================
//
//	テクスチャ分割の指定
//
//========================================
void Animator2D::SetSplit(int nSplitX, int nSplitY) {
	m_nSplitX = nSplitX; m_nSplitY = m_nSplitY;
	Vector3 texSize;
	texSize->x = 1.0f / nSplitX;
	texSize->y = 1.0f / nSplitY;
	m_renderer.lock()->SetTexSize(texSize);
}