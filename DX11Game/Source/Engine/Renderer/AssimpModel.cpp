// Assimpライブラリ用モデルクラス実装 [AssimpModel.cpp]
#include <tuple>
#include <time.h>
#include <assimp/cimport.h>
#include <assimp/Importer.hpp>
#include "AssimpModel.h"
#include "../System/Texture.h"
#include "../System/Shader.h"
#include "../main.h"
#include "mesh.h"
#include "../System/input.h"

#include "../System/debugproc.h"


#ifdef _DEBUG
#pragma comment(lib, "assimp-vc141-mtd")
#pragma comment(lib, "IrrXMLd")
#pragma comment(lib, "zlibstaticd")
#else
#pragma comment(lib, "assimp-vc141-mt")
#pragma comment(lib, "IrrXML")
#pragma comment(lib, "zlibstatic")
#endif

using namespace std;
using namespace DirectX;

#define MAX_BONE_MATRIX	64

// シェーダに渡す値
struct SHADER_GLOBAL {
	XMMATRIX	mWVP;		// ワールド×ビュー×射影行列(転置行列)
	XMMATRIX	mLitWVP;		// ワールド×ビュー×射影行列(転置行列)
	XMMATRIX	mW;			// ワールド行列(転置行列)
	XMMATRIX	mTex;		// テクスチャ行列(転置行列)
	XMVECTOR	vEye;		// 視点座標
	XMVECTOR	vLightDir;	// 光源方向
	XMVECTOR	vLa;		// 光源色(アンビエント)
	XMVECTOR	vLd;		// 光源色(ディフューズ)
	XMVECTOR	vLs;		// 光源色(スペキュラ)
};

// マテリアル (シェーダ用)
struct SHADER_MATERIAL {
	XMVECTOR	vAmbient;	// アンビエント色
	XMVECTOR	vDiffuse;	// ディフューズ色
	XMVECTOR	vSpecular;	// スペキュラ色
	XMVECTOR	vEmissive;	// エミッシブ色
	XMVECTOR	vFlags;		// テクスチャ有無
};

// シェーダに渡すボーン行列配列
struct SHADER_BONE {
	XMMATRIX mBone[MAX_BONE_MATRIX];
	SHADER_BONE()
	{
		for (int i = 0; i < MAX_BONE_MATRIX; i++) {
			mBone[i] = XMMatrixIdentity();
		}
	}
};

const aiMatrix4x4 IdentityMatrix;

// シャドウマップのシェーダー
static ID3D11VertexShader*			g_pShadowVertexShader;		// 頂点シェーダ
static ID3D11PixelShader*			g_pShadowPixelShader;		// ピクセルシェーダ
static ID3D11InputLayout*			g_pShadowInputLayout;		// 頂点フォーマット


// コンストラクタ
AnimEvaluator::AnimEvaluator(const aiAnimation *pAnim)
	: mAnim(pAnim)
	, mLastTime(0.0)
{
	mLastPositions.resize(pAnim->mNumChannels, std::make_tuple(0, 0, 0));
}

// デストラクタ
AnimEvaluator::~AnimEvaluator()
{
	// (空)
}

// 指定された時刻のアニメーショントラックを評価
bool AnimEvaluator::Evaluate(double pTime)
{
	// アニメーション終了フラグ
	bool bExitFlg = false;
	// キーフレームの最大値
	unsigned int nMaxKey = 0;
	for (unsigned int a = 0; a < mAnim->mNumChannels; ++a) 
	{
		const aiNodeAnim* channel = mAnim->mChannels[a];
		// 座標
		if (nMaxKey < channel->mNumPositionKeys)
			nMaxKey = channel->mNumPositionKeys;
		// 回転
		if (nMaxKey < channel->mNumRotationKeys)
			nMaxKey = channel->mNumRotationKeys;
		// 拡縮
		if (nMaxKey < channel->mNumScalingKeys)
			nMaxKey = channel->mNumScalingKeys;
	}

	// 1秒あたりのティックを抽出する。指定されていない場合は既定値を想定。
	double ticksPerSecond = mAnim->mTicksPerSecond != 0.0 ? mAnim->mTicksPerSecond : 25.0;
	// 以降の毎回の計算はティック単位で行う
	pTime *= ticksPerSecond;

	// アニメの持続時間に割り当て
	double time = 0.0f;
	if (mAnim->mDuration > 0.0) {
		time = fmod(pTime, mAnim->mDuration);
	}

	if (mTransforms.size() != mAnim->mNumChannels) {
		mTransforms.resize(mAnim->mNumChannels);
	}

	// 各アニメーションチャンネルの変換マトリックスを算出
	for (unsigned int a = 0; a < mAnim->mNumChannels; ++a) {
		const aiNodeAnim* channel = mAnim->mChannels[a];

		// ******** 座標 ********
		aiVector3D presentPosition(0, 0, 0);
		if (channel->mNumPositionKeys > 0) {
			// 現在のフレーム番号を探す。時間が最後の時間より後の場合は最後の位置から検索し、そうでない場合は最初から検索する。
			// 最初から探す平均的な事例より、ずっと高速に処理する必要がある。
			unsigned int frame = (time >= mLastTime) ? std::get<0>(mLastPositions[a]) : 0;
			while (frame < channel->mNumPositionKeys - 1) {
				if (time < channel->mPositionKeys[frame + 1].mTime) {
					break;
				}
				++frame;
			}

			// 最終フレーム
			if (frame >= nMaxKey - 2)
			{
				bExitFlg = true;
			}

			// このフレームの値と次のフレームの値の間を補間
			unsigned int nextFrame = (frame + 1) % channel->mNumPositionKeys;
			const aiVectorKey& key = channel->mPositionKeys[frame];
			const aiVectorKey& nextKey = channel->mPositionKeys[nextFrame];
			double diffTime = nextKey.mTime - key.mTime;
			if (diffTime < 0.0) {
				diffTime += mAnim->mDuration;
			}
			if (diffTime > 0) {
				float factor = float((time - key.mTime) / diffTime);
				presentPosition = key.mValue + (nextKey.mValue - key.mValue) * factor;
			} else {
				presentPosition = key.mValue;
			}

			std::get<0>(mLastPositions[a]) = frame;
		}

		// ******** 回転 ********
		aiQuaternion presentRotation(1, 0, 0, 0);
		if (channel->mNumRotationKeys > 0) {
			unsigned int frame = (time >= mLastTime) ? std::get<1>(mLastPositions[a]) : 0;
			while (frame < channel->mNumRotationKeys - 1) {
				if (time < channel->mRotationKeys[frame + 1].mTime) {
					break;
				}
				++frame;
			}

			// 最終フレーム
			if (frame >= nMaxKey - 2)
			{
				bExitFlg = true;
			}

			// このフレームの値と次のフレームの値の間を補間
			unsigned int nextFrame = (frame + 1) % channel->mNumRotationKeys;
			const aiQuatKey& key = channel->mRotationKeys[frame];
			const aiQuatKey& nextKey = channel->mRotationKeys[nextFrame];
			double diffTime = nextKey.mTime - key.mTime;
			if (diffTime < 0.0) {
				diffTime += mAnim->mDuration;
			}
			if (diffTime > 0) {
				float factor = float((time - key.mTime) / diffTime);
				aiQuaternion::Interpolate(presentRotation, key.mValue, nextKey.mValue, factor);
			} else {
				presentRotation = key.mValue;
			}

			std::get<1>(mLastPositions[a]) = frame;
		}

		// ******** 拡縮 ********
		aiVector3D presentScaling(1, 1, 1);
		if (channel->mNumScalingKeys > 0) {
			unsigned int frame = (time >= mLastTime) ? std::get<2>(mLastPositions[a]) : 0;
			while (frame < channel->mNumScalingKeys - 1) {
				if (time < channel->mScalingKeys[frame + 1].mTime) {
					break;
				}
				++frame;
			}

			// 最終フレーム
			if (frame >= nMaxKey - 2)
			{
				bExitFlg = true;
			}

			// TODO: 補間が必要では? ここでは線形補間でなく対数かも。
			presentScaling = channel->mScalingKeys[frame].mValue;
			std::get<2>(mLastPositions[a]) = frame;
		}

		// 変換マトリックスを構築
		aiMatrix4x4& mat = mTransforms[a];
		mat = aiMatrix4x4(presentRotation.GetMatrix());
		mat.a1 *= presentScaling.x; mat.b1 *= presentScaling.x; mat.c1 *= presentScaling.x;
		mat.a2 *= presentScaling.y; mat.b2 *= presentScaling.y; mat.c2 *= presentScaling.y;
		mat.a3 *= presentScaling.z; mat.b3 *= presentScaling.z; mat.c3 *= presentScaling.z;
		mat.a4 = presentPosition.x; mat.b4 = presentPosition.y; mat.c4 = presentPosition.z;
	}

	mLastTime = time;

	return bExitFlg;
}

// コンストラクタ
SceneAnimator::SceneAnimator(const aiScene* pScene, size_t pAnimIndex)
	: mScene(pScene)
	, mCurrentAnimIndex(-1)
	, mAnimEvaluator(nullptr)
	, mRootNode(nullptr)
{
	// ボーンのノードテーブルを作成
	for (unsigned int i = 0; i < pScene->mNumMeshes; ++i) {
		const aiMesh* mesh = pScene->mMeshes[i];
		for (unsigned int n = 0; n < mesh->mNumBones; ++n) {
			const aiBone* bone = mesh->mBones[n];

			mBoneNodesByName[bone->mName.data] = pScene->mRootNode->FindNode(bone->mName);
		}
	}

	// 現在のアニメーションを変更すると、このアニメーションのノードツリーも作成される
	SetAnimIndex(pAnimIndex);

	m_dCurrent = m_dLastPlaying = 0.0;
	m_dAnimSpeed = 1.0f;
	m_dAnimTime = 0.0f;
	m_dLastAnimTime = clock();

	m_bStopAnim = false;
}

// デストラクタ
SceneAnimator::~SceneAnimator()
{
	SAFE_DELETE(mRootNode);
	SAFE_DELETE(mAnimEvaluator);
}

// 再生に使用するアニメーションを設定
void SceneAnimator::SetAnimIndex(size_t pAnimIndex)
{
	// 変更無し?
	if (pAnimIndex == static_cast<unsigned int>(mCurrentAnimIndex)) {
		return;
	}

	// 前回のアニメデータを削除
	SAFE_DELETE(mRootNode);
	SAFE_DELETE(mAnimEvaluator);
	mNodesByName.clear();

	mCurrentAnimIndex = static_cast<int>(pAnimIndex);

	// 内部ノードツリー作成
	// アニメーションインデックスが無効な場合でも、現在のシーンを模倣するように変換マトリックスを設定
	mRootNode = CreateNodeTree(mScene->mRootNode, nullptr);

	// アニメーションインデックスが無効?
	if (static_cast<unsigned int>(mCurrentAnimIndex) >= mScene->mNumAnimations) {
		return;
	}

	// このアニメーションの評価クラスを作成
	mAnimEvaluator = new AnimEvaluator(mScene->mAnimations[mCurrentAnimIndex]);
}

// シーンのノード変換マトリックスを計算
bool SceneAnimator::Calculate(double pTime)
{
	// アニメーション無?
	if (!mAnimEvaluator) {
		return false;
	}

	// 現在のローカル変換を計算
	bool bExitFlg = mAnimEvaluator->Evaluate(pTime);

	// 全ノード変換マトリックスを結果で更新
	UpdateTransforms(mRootNode, mAnimEvaluator->GetTransformations());

	return bExitFlg;
}

// 指定されたノードの最新のローカル変換マトリックスを取得
const aiMatrix4x4& SceneAnimator::GetLocalTransform(const aiNode* node) const
{
	NodeMap::const_iterator it = mNodesByName.find(node);
	if (it == mNodesByName.end()) {
		return IdentityMatrix;
	}

	return it->second->mLocalTransform;
}

// 指定されたノードの最新のグローバル変換マトリックスを取得
const aiMatrix4x4& SceneAnimator::GetGlobalTransform(const aiNode* node) const
{
	NodeMap::const_iterator it = mNodesByName.find(node);
	if (it == mNodesByName.end()) {
		return IdentityMatrix;
	}

	return it->second->mGlobalTransform;
}

// メッシュ毎のボーンマトリックスを計算
const std::vector<aiMatrix4x4>& SceneAnimator::GetBoneMatrices(const aiNode* pNode, size_t pMeshIndex /* = 0 */)
{
	//ai_assert(pMeshIndex < pNode->mNumMeshes);
	size_t meshIndex = pNode->mMeshes[pMeshIndex];
	//ai_assert(meshIndex < mScene->mNumMeshes);
	const aiMesh* mesh = mScene->mMeshes[meshIndex];

	// 配列のサイズを変更し、単位マトリックスで初期化
	mTransforms.resize(mesh->mNumBones, aiMatrix4x4());

	// グローバル変換マトリックスの逆行列を計算
	aiMatrix4x4 globalInverseMeshTransform = GetGlobalTransform(pNode);
	globalInverseMeshTransform.Inverse();

	// バインドポーズのメッシュ座標からスキンポーズのメッシュ座標に変換するボーンマトリックスを生成
	// 式は offsetMatrix * currentGlobalTransform * inverseCurrentMeshTransform となる
	for (size_t a = 0; a < mesh->mNumBones; ++a) {
		const aiBone* bone = mesh->mBones[a];
		const aiMatrix4x4& currentGlobalTransform = GetGlobalTransform(mBoneNodesByName[bone->mName.data]);
		mTransforms[a] = globalInverseMeshTransform * currentGlobalTransform * bone->mOffsetMatrix;
	}

	// 結果を返す
	return mTransforms;
}

// 現在のシーンとアニメーションに一致する内部ノード構造を再帰的に生成
SceneAnimNode* SceneAnimator::CreateNodeTree(aiNode* pNode, SceneAnimNode* pParent)
{
	// ノード生成
	SceneAnimNode* internalNode = new SceneAnimNode(pNode->mName.data);
	internalNode->mParent = pParent;
	mNodesByName[pNode] = internalNode;

	// ノードのマトリックスをコピー
	internalNode->mLocalTransform = pNode->mTransformation;
	CalculateGlobalTransform(internalNode);

	// このノードに影響を与えるアニメーショントラックのインデックスを探す
	if (static_cast<unsigned int>(mCurrentAnimIndex) < mScene->mNumAnimations) {
		internalNode->mChannelIndex = -1;
		const aiAnimation* currentAnim = mScene->mAnimations[mCurrentAnimIndex];
		for (unsigned int a = 0; a < currentAnim->mNumChannels; a++) {
			if (currentAnim->mChannels[a]->mNodeName.data == internalNode->mName) {
				internalNode->mChannelIndex = a;
				break;
			}
		}
	}

	// 全ての子ノードに対して、作成された内部ノードを子として割り当て
	for (unsigned int a = 0; a < pNode->mNumChildren; ++a) {
		SceneAnimNode* childNode = CreateNodeTree(pNode->mChildren[a], internalNode);
		internalNode->mChildren.push_back(childNode);
	}

	return internalNode;
}

// 指定されたマトリックス配列から内部ノード変換マトリックスを再帰的に更新
void SceneAnimator::UpdateTransforms(SceneAnimNode* pNode, const std::vector<aiMatrix4x4>& pTransforms)
{
	// ノードのローカル変換マトリックスを更新
	if (pNode->mChannelIndex != -1) {
		//ai_assert(static_cast<unsigned int>(pNode->mChannelIndex) < pTransforms.size());
		pNode->mLocalTransform = pTransforms[pNode->mChannelIndex];
	}

	// グローバル変換マトリックスも更新
	CalculateGlobalTransform(pNode);

	// 全ての子ノードも更新
	for (std::vector<SceneAnimNode*>::iterator it = pNode->mChildren.begin(); it != pNode->mChildren.end(); ++it) {
		UpdateTransforms(*it, pTransforms);
	}
}

// 指定された内部ノードのグローバル変換マトリックスを計算
void SceneAnimator::CalculateGlobalTransform(SceneAnimNode* pInternalNode)
{
	// 全ての親マトリックスを連結して、このノードのグローバル変換マトリックスを取得
	pInternalNode->mGlobalTransform = pInternalNode->mLocalTransform;
	SceneAnimNode* node = pInternalNode->mParent;
	while (node) {
		pInternalNode->mGlobalTransform = node->mLocalTransform * pInternalNode->mGlobalTransform;
		node = node->mParent;
	}
}

// メッシュ クラス
// コンストラクタ
CAssimpMesh::CAssimpMesh(ID3D11Device *pDevice, CAssimpModel* pModel, vector<TAssimpVertex> aVertex, vector<UINT> aIndex, TAssimpMaterial& material)
	: m_pModel(pModel), m_pVertexBuffer(nullptr), m_pIndexBuffer(nullptr), m_pConstantBuffer0(nullptr), m_pConstantBuffer1(nullptr), m_pConstantBufferBone(nullptr)
{
	m_aVertex = aVertex;
	m_aIndex = aIndex;
	m_material = material;
	SetupMesh(pDevice);
}

// デストラクタ
CAssimpMesh::~CAssimpMesh()
{
}

// 解放
void CAssimpMesh::Release()
{
	SAFE_RELEASE(m_pConstantBuffer1);
	SAFE_RELEASE(m_pConstantBuffer0);
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
	m_material.Release();
}

// 頂点バッファ/インデックス バッファ生成
bool CAssimpMesh::SetupMesh(ID3D11Device* pDevice)
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = UINT(sizeof(TAssimpVertex) * m_aVertex.size());
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &m_aVertex[0];

	hr = pDevice->CreateBuffer(&vbd, &initData, &m_pVertexBuffer);
	if (FAILED(hr))
		return false;

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_DYNAMIC;
	ibd.ByteWidth = UINT(sizeof(UINT) * m_aIndex.size());
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ibd.MiscFlags = 0;

	initData.pSysMem = &m_aIndex[0];

	hr = pDevice->CreateBuffer(&ibd, &initData, &m_pIndexBuffer);
	if (FAILED(hr)) {
		return false;
	}

	// コンスタントバッファ作成 変換行列渡し用
	D3D11_BUFFER_DESC cb;
	ZeroMemory(&cb, sizeof(cb));
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(SHADER_GLOBAL);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;
	hr = pDevice->CreateBuffer(&cb, nullptr, &m_pConstantBuffer0);
	if (FAILED(hr)) {
		return false;
	}

	// コンスタントバッファ作成 マテリアル渡し用
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(SHADER_MATERIAL);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;
	hr = pDevice->CreateBuffer(&cb, nullptr, &m_pConstantBuffer1);
	if (FAILED(hr)) {
		return false;
	}

	// コンスタントバッファ ボーン用 作成
	ZeroMemory(&cb, sizeof(cb));
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(SHADER_BONE);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;
	hr = pDevice->CreateBuffer(&cb, nullptr, &m_pConstantBufferBone);

	return SUCCEEDED(hr);
}

// ボーン マトリックス設定
void CAssimpMesh::SetBoneMatrix(ID3D11DeviceContext* pDC, XMFLOAT4X4 mtxBone[])
{
	D3D11_MAPPED_SUBRESOURCE pData;
	if (SUCCEEDED(pDC->Map(m_pConstantBufferBone, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData))) {
		SHADER_BONE sb;
		for (UINT i = 0; i < MAX_BONE_MATRIX; ++i) {
			sb.mBone[i] = XMLoadFloat4x4(&mtxBone[i]);
		}
		memcpy_s(pData.pData, pData.RowPitch, (void*)&sb, sizeof(sb));
		pDC->Unmap(m_pConstantBufferBone, 0);
	}
}

// 描画
void CAssimpMesh::Draw(ID3D11DeviceContext* pDC, XMFLOAT4X4& m44World, EByOpacity byOpacity)
{
	// ユーザ定義マテリアル
	TAssimpMaterial* pMaterial = m_pModel->GetMaterial();
	if (!pMaterial)
		pMaterial = &m_material;
	// 透明度による描画切替
	switch (byOpacity) {
	case eOpacityOnly:
		if (pMaterial->Kd.w < 1.0f) return;
		break;
	case eTransparentOnly:
		if (pMaterial->Kd.w >= 1.0f) return;
		break;
	default:
		break;
	}
	// テクスチャフラグ
	XMFLOAT4X4 mtxTexture = m_pModel->GetTextureMatrix();
	XMUINT4 vFlags(0, 0, 0, 0);
	if (pMaterial->pTexture) {
		vFlags.x = 1;
		if (pMaterial->pTexEmmisive)
			vFlags.y = 1;
		if (pMaterial->pTexTransparent)
			vFlags.z = 1;
		if (pMaterial->pTexSpecular)
			vFlags.w = 1;
	} else {
		mtxTexture._44 = 0.0f;
	}
	// 定数領域更新
	D3D11_MAPPED_SUBRESOURCE pData;
	if (SUCCEEDED(pDC->Map(m_pConstantBuffer0, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData))) {
		SHADER_GLOBAL sg;
		CLight* pLight = m_pModel->GetLight();
		CCamera* pCamera = m_pModel->GetCamera();
		XMMATRIX mtxWorld = XMLoadFloat4x4(&m44World);
		XMMATRIX mtxView = XMLoadFloat4x4(&pCamera->GetViewMatrix());
		XMMATRIX mtxProj = XMLoadFloat4x4(&pCamera->GetProjMatrix());
		sg.mW = XMMatrixTranspose(mtxWorld);
		XMMATRIX mtxTex = XMLoadFloat4x4(&mtxTexture);
		sg.mTex = XMMatrixTranspose(mtxTex);
		sg.mWVP = mtxWorld * mtxView * mtxProj;
		sg.mWVP = XMMatrixTranspose(sg.mWVP);
		sg.vEye = XMLoadFloat3(&pCamera->GetPos());

		mtxView = XMLoadFloat4x4(&pLight->GetViewMatrix());
		sg.mLitWVP = mtxWorld * mtxView * mtxProj;
		sg.mLitWVP = XMMatrixTranspose(sg.mLitWVP);
		//sg.mLitWVP = sg.mWVP;

		sg.vLightDir = XMLoadFloat3(&pLight->GetDir());
		sg.vLd = XMLoadFloat4(&pLight->GetDiffuse());
		sg.vLa = XMLoadFloat4(&pLight->GetAmbient());
		sg.vLs = XMLoadFloat4(&pLight->GetSpecular());
		memcpy_s(pData.pData, pData.RowPitch, (void*)&sg, sizeof(sg));
		pDC->Unmap(m_pConstantBuffer0, 0);
	}
	pDC->VSSetConstantBuffers(0, 1, &m_pConstantBuffer0);
	pDC->PSSetConstantBuffers(0, 1, &m_pConstantBuffer0);

	// 頂点バッファ/インデックス バッファをセット
	UINT stride = sizeof(TAssimpVertex);
	UINT offset = 0;
	pDC->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	pDC->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// マテリアルの各要素をシェーダに渡す
	if (SUCCEEDED(pDC->Map(m_pConstantBuffer1, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData))) {
		SHADER_MATERIAL sg;
		sg.vAmbient = XMLoadFloat4(&pMaterial->Ka);
		sg.vDiffuse = XMLoadFloat4(&pMaterial->Kd);
		sg.vSpecular = XMLoadFloat4(&pMaterial->Ks);
		sg.vEmissive = XMLoadFloat4(&pMaterial->Ke);
		sg.vFlags = XMLoadUInt4(&vFlags);
		memcpy_s(pData.pData, pData.RowPitch, (void*)&sg, sizeof(sg));
		pDC->Unmap(m_pConstantBuffer1, 0);
	}
	pDC->PSSetConstantBuffers(1, 1, &m_pConstantBuffer1);

	// テクスチャをシェーダに渡す
	if (pMaterial->pTexture) {
		pDC->PSSetShaderResources(0, 1, &pMaterial->pTexture);
		if (pMaterial->pTexEmmisive)
			pDC->PSSetShaderResources(1, 1, &pMaterial->pTexEmmisive);
		if (pMaterial->pTexTransparent)
			pDC->PSSetShaderResources(2, 1, &pMaterial->pTexTransparent);
		if (pMaterial->pTexSpecular)
			pDC->PSSetShaderResources(3, 1, &pMaterial->pTexSpecular);
	}
	// シャドウマップ
	pDC->PSSetShaderResources(4, 1, &g_pShadowResourceView);

	// ボーンをシェーダに渡す
	pDC->VSSetConstantBuffers(2, 1, &m_pConstantBufferBone);

	// 描画
	pDC->DrawIndexed(UINT(m_aIndex.size()), 0, 0);
}

// 描画
void CAssimpMesh::DrawShadow(ID3D11DeviceContext* pDC, XMFLOAT4X4& m44World, EByOpacity byOpacity)
{

	pDC->OMSetRenderTargets(0, nullptr, g_pShadowDepthStencilView);
	pDC->VSSetShader(g_pShadowVertexShader, nullptr, 0);
	pDC->PSSetShader(nullptr, nullptr, 0);
	pDC->IASetInputLayout(g_pShadowInputLayout);

	// ユーザ定義マテリアル
	TAssimpMaterial* pMaterial = m_pModel->GetMaterial();
	if (!pMaterial)
		pMaterial = &m_material;
	// 透明度による描画切替
	switch (byOpacity) {
	case eOpacityOnly:
		if (pMaterial->Kd.w < 1.0f) return;
		break;
	case eTransparentOnly:
		if (pMaterial->Kd.w >= 1.0f) return;
		break;
	default:
		break;
	}
	// テクスチャフラグ
	XMFLOAT4X4 mtxTexture = m_pModel->GetTextureMatrix();
	XMUINT4 vFlags(0, 0, 0, 0);
	if (pMaterial->pTexture) {
		vFlags.x = 1;
		if (pMaterial->pTexEmmisive)
			vFlags.y = 1;
		if (pMaterial->pTexTransparent)
			vFlags.z = 1;
		if (pMaterial->pTexSpecular)
			vFlags.w = 1;
	}
	else {
		mtxTexture._44 = 0.0f;
	}
	// 定数領域更新
	D3D11_MAPPED_SUBRESOURCE pData;
	if (SUCCEEDED(pDC->Map(m_pConstantBuffer0, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData))) {
		SHADER_GLOBAL sg;
		CLight* pLight = m_pModel->GetLight();
		CCamera* pCamera = m_pModel->GetCamera();
		XMMATRIX mtxWorld = XMLoadFloat4x4(&m44World);
		XMMATRIX mtxView = XMLoadFloat4x4(&pLight->GetViewMatrix());
		XMMATRIX mtxProj = XMLoadFloat4x4(&pCamera->GetProjMatrix());
		sg.mW = XMMatrixTranspose(mtxWorld);
		XMMATRIX mtxTex = XMLoadFloat4x4(&mtxTexture);
		sg.mTex = XMMatrixTranspose(mtxTex);
		sg.mWVP = mtxWorld * mtxView * mtxProj;
		sg.mWVP = XMMatrixTranspose(sg.mWVP);
		sg.vEye = XMLoadFloat3(&pCamera->GetPos());
		//sg.mWVP = XMMatrixTranspose(mtxWorld * XMLoadFloat4x4(&pLight->GetViewMatrix()) * XMLoadFloat4x4(&pCamera->GetProjMatrix()));
		//sg.mLitWVP = XMMatrixTranspose(mtxWorld * XMLoadFloat4x4(&pLight->GetViewMatrix()) * XMLoadFloat4x4(&pCamera->GetProjMatrix()));
		sg.mLitWVP = sg.mWVP;

		sg.vLightDir = XMLoadFloat3(&pLight->GetDir());
		sg.vLd = XMLoadFloat4(&pLight->GetDiffuse());
		sg.vLa = XMLoadFloat4(&pLight->GetAmbient());
		sg.vLs = XMLoadFloat4(&pLight->GetSpecular());
		memcpy_s(pData.pData, pData.RowPitch, (void*)&sg, sizeof(sg));
		pDC->Unmap(m_pConstantBuffer0, 0);
	}
	pDC->VSSetConstantBuffers(0, 1, &m_pConstantBuffer0);
	pDC->PSSetConstantBuffers(0, 1, &m_pConstantBuffer0);

	// 頂点バッファ/インデックス バッファをセット
	UINT stride = sizeof(TAssimpVertex);
	UINT offset = 0;
	pDC->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	pDC->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// マテリアルの各要素をシェーダに渡す
	if (SUCCEEDED(pDC->Map(m_pConstantBuffer1, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData))) {
		SHADER_MATERIAL sg;
		sg.vAmbient = XMLoadFloat4(&pMaterial->Ka);
		sg.vDiffuse = XMLoadFloat4(&pMaterial->Kd);
		sg.vSpecular = XMLoadFloat4(&pMaterial->Ks);
		sg.vEmissive = XMLoadFloat4(&pMaterial->Ke);
		sg.vFlags = XMLoadUInt4(&vFlags);
		memcpy_s(pData.pData, pData.RowPitch, (void*)&sg, sizeof(sg));
		pDC->Unmap(m_pConstantBuffer1, 0);
	}
	pDC->PSSetConstantBuffers(1, 1, &m_pConstantBuffer1);

	// テクスチャをシェーダに渡す
	if (pMaterial->pTexture) {
		pDC->PSSetShaderResources(0, 1, &pMaterial->pTexture);
		if (pMaterial->pTexEmmisive)
			pDC->PSSetShaderResources(1, 1, &pMaterial->pTexEmmisive);
		if (pMaterial->pTexTransparent)
			pDC->PSSetShaderResources(2, 1, &pMaterial->pTexTransparent);
		if (pMaterial->pTexSpecular)
			pDC->PSSetShaderResources(3, 1, &pMaterial->pTexSpecular);
	}
	// シャドウマップ
	pDC->PSSetShaderResources(4, 1, &g_pShadowResourceView);

	// ボーンをシェーダに渡す
	pDC->VSSetConstantBuffers(2, 1, &m_pConstantBufferBone);

	// 描画
	pDC->DrawIndexed(UINT(m_aIndex.size()), 0, 0);
	//// シャドウマップ
	//// 各ターゲットビューをレンダーターゲットに設定
	//pDC->OMSetRenderTargets(0, nullptr, g_pShadowDepthStencilView);

	//// シェーダ設定
	//pDC->VSSetShader(g_pShadowVertexShader, nullptr, 0);
	//pDC->PSSetShader(nullptr, nullptr, 0);
	//pDC->IASetInputLayout(g_pShadowInputLayout);

	//// 頂点バッファをセット
	//UINT stride = sizeof(VERTEX_3D);
	//UINT offset = 0;
	//pDC->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	//// インデックスバッファをセット
	//pDC->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//// ユーザ定義マテリアル
	//TAssimpMaterial* pMaterial = m_pModel->GetMaterial();
	//if (!pMaterial)
	//	pMaterial = &m_material;
	//// 透明度による描画切替
	//switch (byOpacity) {
	//case eOpacityOnly:
	//	if (pMaterial->Kd.w < 1.0f) return;
	//	break;
	//case eTransparentOnly:
	//	if (pMaterial->Kd.w >= 1.0f) return;
	//	break;
	//default:
	//	break;
	//}
	//// テクスチャフラグ
	//XMFLOAT4X4 mtxTexture = m_pModel->GetTextureMatrix();
	//XMUINT4 vFlags(0, 0, 0, 0);
	//if (pMaterial->pTexture) {
	//	vFlags.x = 1;
	//	if (pMaterial->pTexEmmisive)
	//		vFlags.y = 1;
	//	if (pMaterial->pTexTransparent)
	//		vFlags.z = 1;
	//	if (pMaterial->pTexSpecular)
	//		vFlags.w = 1;
	//}
	//else {
	//	mtxTexture._44 = 0.0f;
	//}

	//// 定数領域更新
	//D3D11_MAPPED_SUBRESOURCE pData;
	//if (SUCCEEDED(pDC->Map(m_pConstantBuffer0, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData))) {
	//	SHADER_GLOBAL sg;
	//	CCamera* pCamera = m_pModel->GetCamera();
	//	CLight* pLight = m_pModel->GetLight();
	//	XMMATRIX mtxWorld = XMLoadFloat4x4(&m44World);
	//	XMMATRIX mtxView = XMLoadFloat4x4(&pCamera->GetProjMatrix());
	//	XMMATRIX mtxProj = XMLoadFloat4x4(&pCamera->GetProjMatrix());
	//	sg.mW = XMMatrixTranspose(mtxWorld);
	//	XMMATRIX mtxTex = XMLoadFloat4x4(&mtxTexture);
	//	sg.mTex = XMMatrixTranspose(mtxTex);
	//	sg.mWVP = mtxWorld * mtxView * mtxProj;
	//	sg.mWVP = XMMatrixTranspose(sg.mWVP);
	//	sg.mLitWVP = sg.mWVP;
	//	sg.vEye = XMLoadFloat3(&pCamera->GetPos());
	//	sg.vLightDir = XMLoadFloat3(&pLight->GetDir());
	//	sg.vLd = XMLoadFloat4(&pLight->GetDiffuse());
	//	sg.vLa = XMLoadFloat4(&pLight->GetAmbient());
	//	sg.vLs = XMLoadFloat4(&pLight->GetSpecular());
	//	memcpy_s(pData.pData, pData.RowPitch, (void*)&sg, sizeof(sg));
	//	pDC->Unmap(m_pConstantBuffer0, 0);
	//}
	//pDC->VSSetConstantBuffers(0, 1, &m_pConstantBuffer0);

	//// ボーンをシェーダに渡す
	//pDC->VSSetConstantBuffers(2, 1, &m_pConstantBufferBone);


	//pDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//// 描画
	//pDC->DrawIndexed(UINT(m_aIndex.size()), 0, 0);
}

// モデル クラス
// 静的メンバ変数
ID3D11InputLayout* CAssimpModel::m_pVertexLayout;
ID3D11VertexShader* CAssimpModel::m_pVertexShader;
ID3D11PixelShader* CAssimpModel::m_pPixelShader;
ID3D11SamplerState* CAssimpModel::m_pSampleLinear;

//using ModelKey = std::pair<std::string, std::unique_ptr<CAssimpModel>>;


// コンストラクタ
CAssimpModel::CAssimpModel() : m_pMaterial(nullptr), m_pLight(nullptr), m_pCamera(nullptr), m_pAnimator(nullptr), m_pScene(nullptr)
{
	XMStoreFloat4x4(&m_mtxTexture, XMMatrixIdentity());
	XMStoreFloat4x4(&m_mtxWorld, XMMatrixIdentity());
}

// デストラクタ
CAssimpModel::~CAssimpModel()
{
	if (m_pMaterial)
	{
		delete m_pMaterial;
	}
}

// テクスチャ マトリックス設定
void CAssimpModel::SetTextureMatrix(DirectX::XMFLOAT4X4& mtxTexture)
{
	m_mtxTexture = mtxTexture;
}

// テクスチャ マトリックス取得
XMFLOAT4X4& CAssimpModel::GetTextureMatrix()
{
	return m_mtxTexture;
}

// シェーダ初期化
bool CAssimpModel::InitShader(ID3D11Device* pDevice)
{
	// シェーダ読み込み
	HRESULT hr = LoadShader("AssimpVertex", "AssimpPixel",
		&m_pVertexShader, &m_pVertexLayout, &m_pPixelShader);
	if (FAILED(hr)) {
		MessageBoxW(0, L"hlsl読み込み失敗", nullptr, MB_OK);
		return false;
	}

	// シャドウマップシェーダー
	hr = LoadShader("AssimpVertex", "AssimpPixel",
		&g_pShadowVertexShader, &g_pShadowInputLayout, &g_pShadowPixelShader);
	if (FAILED(hr)) {
		MessageBoxW(0, L"hlsl読み込み失敗", nullptr, MB_OK);
		return false;
	}

	// テクスチャ用サンプラ作成
	CD3D11_DEFAULT def;
	CD3D11_SAMPLER_DESC sd(def);
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	hr = pDevice->CreateSamplerState(&sd, &m_pSampleLinear);
	if (FAILED(hr)) {
		MessageBoxW(0, L"テクスチャ用サンプラ作成失敗", nullptr, MB_OK);
		return false;
	}

	return true;
}

// シェーダ終了処理
void CAssimpModel::UninitShader()
{
	SAFE_RELEASE(m_pSampleLinear);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pVertexLayout);
	SAFE_RELEASE(m_pVertexShader);
}

// モデル読み込み
bool CAssimpModel::Load(ID3D11Device* pDevice, ID3D11DeviceContext* pDC, std::string filename)
{
	// default pp steps
	static unsigned int ppsteps = aiProcess_CalcTangentSpace | // calculate tangents and bitangents if possible
		aiProcess_JoinIdenticalVertices | // join identical vertices/ optimize indexing
		aiProcess_ValidateDataStructure | // perform a full validation of the loader's output
		aiProcess_ImproveCacheLocality | // improve the cache locality of the output vertices
		aiProcess_RemoveRedundantMaterials | // remove redundant materials
		aiProcess_FindDegenerates | // remove degenerated polygons from the import
		aiProcess_FindInvalidData | // detect invalid model data, such as invalid normal vectors
		aiProcess_GenUVCoords | // convert spherical, cylindrical, box and planar mapping to proper UVs
		aiProcess_TransformUVCoords | // preprocess UV transformations (scaling, translation ...)
		aiProcess_FindInstances | // search for instanced meshes and remove them by references to one master
		aiProcess_LimitBoneWeights | // limit bone weights to 4 per vertex
		aiProcess_OptimizeMeshes | // join small meshes, if possible;
		aiProcess_SplitByBoneCount | // split meshes with too many bones. Necessary for our (limited) hardware skinning shader
		0;

	Release();

	aiPropertyStore* props = aiCreatePropertyStore();
	aiSetImportPropertyInteger(props, AI_CONFIG_IMPORT_TER_MAKE_UVS, 1);
	//aiSetImportPropertyFloat(props, AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, g_smoothAngle);
	//aiSetImportPropertyInteger(props, AI_CONFIG_PP_SBP_REMOVE, nopointslines ? aiPrimitiveType_LINE | aiPrimitiveType_POINT : 0);
	aiSetImportPropertyInteger(props, AI_CONFIG_GLOB_MEASURE_TIME, 1);
	m_pScene = (aiScene*)aiImportFileExWithProperties(filename.c_str(),
		ppsteps |
		aiProcess_GenSmoothNormals | // generate smooth normal vectors if not existing
		aiProcess_SplitLargeMeshes | // split large, unrenderable meshes into submeshes
		aiProcess_Triangulate | // triangulate polygons with more than 3 edges
		aiProcess_ConvertToLeftHanded | // convert everything to D3D left handed space
		aiProcess_SortByPType | // make 'clean' meshes which consist of a single typ of primitives
		0, NULL, props);
	aiReleasePropertyStore(props);
	if (!m_pScene)
		return false;

	char szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFName[_MAX_FNAME], szExt[_MAX_EXT];
	_splitpath_s(filename.c_str(), szDrive, _MAX_DRIVE, szDir, _MAX_DIR, szFName, _MAX_FNAME, szExt, _MAX_EXT);
	m_directory = szDrive;
	m_directory += szDir;
	m_fname = szFName;
	m_ext = szExt;

	processNode(pDevice, m_pScene->mRootNode);

	if (m_pScene->HasAnimations())
		m_pAnimator = new SceneAnimator(m_pScene);

	ScaleAsset();

	//m_assimpModelPool.insert(ModelKey(filename, unique_ptr<CAssimpModel>(this)));


	return true;
}

// アニメーションの再生場所
void CAssimpModel::SetStartAnimTime(double dTime)
{
	if (m_pAnimator) {
		m_pAnimator->m_dAnimTime = dTime;
		m_pAnimator->m_dCurrent = m_pAnimator->m_dAnimTime;

		m_pAnimator->m_dLastPlaying = m_pAnimator->m_dCurrent;
		m_pAnimator->m_dLastAnimTime = clock();
	}
}

void CAssimpModel::SetAnimSpeed(double dSpeed)
{
	if (m_pAnimator) {
		m_pAnimator->m_dAnimSpeed = dSpeed;
	}
}

void CAssimpModel::SetStopAnim(bool bStop)
{
	if (m_pAnimator) {
		m_pAnimator->m_bStopAnim = bStop;
	}
}

// 描画
bool CAssimpModel::Draw(ID3D11DeviceContext* pDC, XMFLOAT4X4& mtxWorld, EByOpacity byOpacity, bool bShadowMode)
{
	if (!m_pScene) return false;

	bool bExitFlg = false;

	// アニメーション更新
	if (m_pAnimator && !m_pAnimator->m_bStopAnim) {
		m_pAnimator->m_dCurrent += (m_pAnimator->m_dAnimTime * m_pAnimator->m_dAnimSpeed) / double(CLOCKS_PER_SEC) - m_pAnimator->m_dLastPlaying;
		double time = m_pAnimator->m_dCurrent;

		bExitFlg = m_pAnimator->Calculate(time);
		m_pAnimator->m_dLastPlaying = m_pAnimator->m_dCurrent;

		// 差分を加算
		m_pAnimator->m_dAnimTime += clock() - m_pAnimator->m_dLastAnimTime;
		m_pAnimator->m_dLastAnimTime = clock();
	}


	if (bShadowMode)
	{
		// 各ターゲットビューをレンダーターゲットに設定
		pDC->OMSetRenderTargets(0, nullptr, g_pShadowDepthStencilView);
	}
	else
	{
		// レンダーターゲット
		SetRenderTarget();
	}


	m_mtxWorld = mtxWorld;
	// 使用するシェーダーの登録	
	pDC->VSSetShader(m_pVertexShader, nullptr, 0);
	pDC->PSSetShader(m_pPixelShader, nullptr, 0);
	// 頂点インプットレイアウトをセット
	pDC->IASetInputLayout(m_pVertexLayout);
	// プリミティブ形状をセット
	pDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// テクスチャサンプラをセット
	pDC->PSSetSamplers(0, 1, &m_pSampleLinear);
	pDC->PSSetSamplers(1, 1, &g_pShadowSamplerState);
	// ノード単位で描画
	aiMatrix4x4* piMatrix = (aiMatrix4x4*)&m_mtxWorld;
	DrawNode(pDC, m_pScene->mRootNode, *piMatrix, byOpacity, bShadowMode);

	
	return bExitFlg;
}

// 描画
void CAssimpModel::DrawNode(ID3D11DeviceContext* pDC, aiNode* piNode, const aiMatrix4x4& piMatrix, EByOpacity byOpacity, bool bShadowMode)
{
	aiMatrix4x4 aiMe;
	if (m_pAnimator) {
		aiMe = m_pAnimator->GetGlobalTransform(piNode);
		aiMe.Transpose();
	}
	aiMe *= piMatrix;

	XMFLOAT4X4 mtxWorld(aiMe.a1, aiMe.a2, aiMe.a3, aiMe.a4,
		aiMe.b1, aiMe.b2, aiMe.b3, aiMe.b4,
		aiMe.c1, aiMe.c2, aiMe.c3, aiMe.c4,
		aiMe.d1, aiMe.d2, aiMe.d3, aiMe.d4);

	for (UINT i = 0; i < piNode->mNumMeshes; ++i) {
		const aiMesh* mesh = m_pScene->mMeshes[piNode->mMeshes[i]];
		CAssimpMesh& helper = m_aMesh[piNode->mMeshes[i]];

		// ボーンマトリックス更新
		// (おそらくここで実行するのは不適切だが、解決方法が不明)
		if (mesh->HasBones()) {
			static float matrices[4 * 4 * MAX_BONE_MATRIX];
			float* tempmat = matrices;
			const std::vector<aiMatrix4x4>& boneMats = m_pAnimator->GetBoneMatrices(piNode, i);

			for (UINT a = 0; a < mesh->mNumBones; a++) {
				const aiMatrix4x4& mat = boneMats[a];
				*tempmat++ = mat.a1; *tempmat++ = mat.a2; *tempmat++ = mat.a3; *tempmat++ = mat.a4;
				*tempmat++ = mat.b1; *tempmat++ = mat.b2; *tempmat++ = mat.b3; *tempmat++ = mat.b4;
				*tempmat++ = mat.c1; *tempmat++ = mat.c2; *tempmat++ = mat.c3; *tempmat++ = mat.c4;
				*tempmat++ = mat.d1; *tempmat++ = mat.d2; *tempmat++ = mat.d3; *tempmat++ = mat.d4;
			}

			helper.SetBoneMatrix(pDC, (XMFLOAT4X4*)matrices);
		} else {
			static XMFLOAT4X4 identity[MAX_BONE_MATRIX] = { XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1) };
			helper.SetBoneMatrix(pDC, identity);
		}

		if (bShadowMode)
		{
			helper.DrawShadow(pDC, mtxWorld, byOpacity);
		}
		else
		{
			helper.Draw(pDC, mtxWorld, byOpacity);
		}
	}

	// 全ての子ノードをレンダリング
	for (UINT i = 0; i < piNode->mNumChildren; ++i)
		DrawNode(pDC, piNode->mChildren[i], piMatrix, byOpacity, bShadowMode);
}

// 解放
void CAssimpModel::Release()
{
	for (int i = 0; i < m_aMesh.size(); ++i) {
		m_aMesh[i].Release();
	}
	SAFE_DELETE(m_pAnimator);
	if (m_pScene) {
		aiReleaseImport(m_pScene);
		m_pScene = nullptr;
	}
}

// 全メッシュ取り込み
void CAssimpModel::processNode(ID3D11Device* pDevice, aiNode* node)
{
	for (UINT i = 0; i < node->mNumMeshes; ++i) {
		aiMesh* mesh = m_pScene->mMeshes[node->mMeshes[i]];
		m_aMesh.push_back(processMesh(pDevice, mesh));
	}

	for (UINT i = 0; i < node->mNumChildren; ++i) {
		processNode(pDevice, node->mChildren[i]);
	}
}

// メッシュ展開
CAssimpMesh CAssimpModel::processMesh(ID3D11Device* pDevice, aiMesh* mesh)
{
	vector<TAssimpVertex> aVertex;
	vector<UINT> aIndex;
	TAssimpMaterial material;

	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* mat = m_pScene->mMaterials[mesh->mMaterialIndex];

		if (m_textype.empty()) m_textype = determineTextureType(mat);
	}

	// 全頂点のウェイトを収集(高速化のためチェック無)
	std::vector<std::vector<aiVertexWeight> >weightsPerVertex(mesh->mNumVertices);
	for (unsigned int a = 0; a < mesh->mNumBones; a++) {
		const aiBone* bone = mesh->mBones[a];
		for (unsigned int b = 0; b < bone->mNumWeights; b++)
			weightsPerVertex[bone->mWeights[b].mVertexId].push_back(aiVertexWeight(a, bone->mWeights[b].mWeight));
	}

	// メッシュの各頂点をウォークスルー
	for (UINT i = 0; i < mesh->mNumVertices; ++i) {
		TAssimpVertex vertex;

		vertex.vPos.x = mesh->mVertices[i].x;
		vertex.vPos.y = mesh->mVertices[i].y;
		vertex.vPos.z = mesh->mVertices[i].z;

		vertex.vNorm.x = mesh->mNormals[i].x;
		vertex.vNorm.y = mesh->mNormals[i].y;
		vertex.vNorm.z = mesh->mNormals[i].z;

		if (mesh->mTextureCoords[0]) {
			vertex.vTex.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.vTex.y = (float)mesh->mTextureCoords[0][i].y;
		}

		// ボーンのインデックスとウェイト
		unsigned int boneIndices[4] = { 0, 0, 0, 0 };
		float boneWeights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		if (mesh->HasBones()) {
			for (unsigned int a = 0; a < weightsPerVertex[i].size() && a < 4; a++) {
				boneIndices[a] = weightsPerVertex[i][a].mVertexId;
				boneWeights[a] = weightsPerVertex[i][a].mWeight;
			}
		}
		memcpy(vertex.uBoneIndex, boneIndices, sizeof(boneIndices));
		memcpy(vertex.fBoneWeight, boneWeights, sizeof(boneWeights));

		aVertex.push_back(vertex);
	}

	for (UINT i = 0; i < mesh->mNumFaces; ++i) {
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; ++j)
			aIndex.push_back(face.mIndices[j]);
	}

	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* pMaterial = m_pScene->mMaterials[mesh->mMaterialIndex];

		material = loadMaterial(pDevice, pMaterial, mesh);
	}

	return CAssimpMesh(pDevice, this, aVertex, aIndex, material);
}

// マテリアル読込
TAssimpMaterial CAssimpModel::loadMaterial(ID3D11Device* pDevice, aiMaterial* mat, aiMesh* mesh)
{
	TAssimpMaterial material;
	aiColor4D color;
	HRESULT hr = S_OK;

	// DIFFUSE COLOR
	if (AI_SUCCESS != aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color)) {
        color.r = 1.0f;
        color.g = 1.0f;
        color.b = 1.0f;
        color.a = material.Kd.w;
    }
	material.Kd = XMFLOAT4(color.r, color.g, color.b, color.a);

	// SPECULAR COLOR
	if (AI_SUCCESS != aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &color)) {
		color.r = 0.0f;
		color.g = 0.0f;
		color.b = 0.0f;
		color.a = 1.0f;
	}
	material.Ks = XMFLOAT4(color.r, color.g, color.b, material.Ks.w);

	// AMBIENT COLOR
	if (AI_SUCCESS != aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT, &color)) {
		color.r = 0.0f;
		color.g = 0.0f;
		color.b = 0.0f;
		color.a = 0.0f;
	}
	material.Ka = XMFLOAT4(color.r, color.g, color.b, color.a);

	// EMISSIVE COLOR
	if (AI_SUCCESS != aiGetMaterialColor(mat, AI_MATKEY_COLOR_EMISSIVE, &color)) {
		color.r = 0.0f;
		color.g = 0.0f;
		color.b = 0.0f;
		color.a = 0.0f;
	}
	material.Ke = XMFLOAT4(color.r, color.g, color.b, color.a);

	// Opacity
	if (AI_SUCCESS != aiGetMaterialFloat(mat, AI_MATKEY_OPACITY, &material.Kd.w)) {
		//material.Kd.w = 1.0f;
	}

	// Shininess
	if (AI_SUCCESS != aiGetMaterialFloat(mat, AI_MATKEY_SHININESS, &material.Ks.w)) {
		//material.Ks.w = 1.0f;
	}

	aiString szPath;
	aiTextureMapMode mapU(aiTextureMapMode_Wrap), mapV(aiTextureMapMode_Wrap);
	bool bib = false;
	if (mesh->mTextureCoords[0]) {
		// DIFFUSE TEXTURE
		if (AI_SUCCESS == aiGetMaterialString(mat, AI_MATKEY_TEXTURE_DIFFUSE(0), &szPath)) {
			LoadTexture(pDevice, szPath, &material.pTexture);

			aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_U_DIFFUSE(0), (int*)&mapU);
			aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_V_DIFFUSE(0), (int*)&mapV);
		}

		// SPECULAR TEXTURE
		if (AI_SUCCESS == aiGetMaterialString(mat, AI_MATKEY_TEXTURE_SPECULAR(0), &szPath)) {
			LoadTexture(pDevice, szPath, &material.pTexSpecular);
		}

		// EMISSIVE TEXTURE
		if (AI_SUCCESS == aiGetMaterialString(mat, AI_MATKEY_TEXTURE_EMISSIVE(0), &szPath)) {
			LoadTexture(pDevice, szPath, &material.pTexEmmisive);
		}

		// NORMAL TEXTURE
		if (AI_SUCCESS == aiGetMaterialString(mat, AI_MATKEY_TEXTURE_NORMALS(0), &szPath)) {
			LoadTexture(pDevice, szPath, &material.pTexNormal);
		}

		// AMBIENT TEXTURE
		if (AI_SUCCESS == aiGetMaterialString(mat, AI_MATKEY_TEXTURE_AMBIENT(0), &szPath)) {
			LoadTexture(pDevice, szPath, &material.pTexAmbient);
		}

		// OPACITY TEXTURE
		if (AI_SUCCESS == aiGetMaterialString(mat, AI_MATKEY_TEXTURE_OPACITY(0), &szPath)) {
			LoadTexture(pDevice, szPath, &material.pTexTransparent);
		} else {
			int flags = 0;
			aiGetMaterialInteger(mat, AI_MATKEY_TEXFLAGS_DIFFUSE(0), &flags);

			// try to find out whether the diffuse texture has any
			// non-opaque pixels. If we find a few, use it as opacity texture
			if (material.pTexture && !(flags & aiTextureFlags_IgnoreAlpha) && HasAlphaPixels(material.pTexture)) {
				int iVal;

				// NOTE: This special value is set by the tree view if the user
				// manually removes the alpha texture from the view ...
				if (AI_SUCCESS != aiGetMaterialInteger(mat, "no_a_from_d", 0, 0, &iVal)) {
					material.pTexTransparent = material.pTexture;
					material.pTexTransparent->AddRef();
				}
			}
		}
	}

	return material;
}

// テクスチャ タイプ決定
string CAssimpModel::determineTextureType(aiMaterial* mat)
{
	aiString textypeStr;
	mat->GetTexture(aiTextureType_DIFFUSE, 0, &textypeStr);
	string textypeteststr = textypeStr.C_Str();
	if (textypeteststr == "*0" || textypeteststr == "*1" || textypeteststr == "*2" || textypeteststr == "*3" || textypeteststr == "*4" || textypeteststr == "*5") {
		if (m_pScene->mTextures[0]->mHeight == 0) {
			return "embedded compressed texture";
		} else {
			return "embedded non-compressed texture";
		}
	}
	if (textypeteststr.find('.') != string::npos) {
		return "textures are on disk";
	}
	return "";
}

// テクスチャ インデックス取得
int CAssimpModel::getTextureIndex(aiString* str)
{
	string tistr;
	tistr = str->C_Str();
	tistr = tistr.substr(1);
	return stoi(tistr);
}

// モデル データ内包テクスチャ読込
ID3D11ShaderResourceView* CAssimpModel::getTextureFromModel(ID3D11Device* pDevice, int textureindex)
{
	HRESULT hr = S_OK;
	ID3D11ShaderResourceView* texture;

	int* size = reinterpret_cast<int*>(&m_pScene->mTextures[textureindex]->mWidth);
	hr = CreateTextureFromMemory(pDevice, reinterpret_cast<unsigned char*>(m_pScene->mTextures[textureindex]->pcData), *size, &texture);
	if (FAILED(hr))
		MessageBoxA(NULL, "Texture couldn't be created from memory!", "Error!", MB_ICONERROR | MB_OK);

	return texture;
}

// テクスチャ読込
void CAssimpModel::LoadTexture(ID3D11Device* pDevice, aiString& szPath, ID3D11ShaderResourceView** ppTexture)
{
	if (m_textype == "embedded compressed texture") {
		int textureindex = getTextureIndex(&szPath);
		*ppTexture = getTextureFromModel(pDevice, textureindex);
	} else {
		string filename = string(szPath.C_Str());
		filename = m_directory + filename;
		HRESULT hr = S_OK;
		*ppTexture = CTexture::GetTexture(pDevice, filename.c_str());
		if (*ppTexture == nullptr) hr = E_FAIL;

		if (FAILED(hr)) {
			char szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFName[_MAX_FNAME], szExt[_MAX_EXT];
			_splitpath_s(szPath.C_Str(), szDrive, _MAX_DRIVE, szDir, _MAX_DIR, szFName, _MAX_FNAME, szExt, _MAX_EXT);
			filename = m_directory + szFName + szExt;
			*ppTexture = CTexture::GetTexture(pDevice, filename.c_str());
			if (*ppTexture) hr = S_OK;
			else { MessageBoxA(GetMainWnd(), "テクスチャ読み込みエラー", filename.c_str(), MB_OK); ; }

			if (FAILED(hr)) {
				if (m_ext == ".fbx") {
					filename = m_directory + m_fname + ".fbx/" + szFName + szExt;
					*ppTexture = CTexture::GetTexture(pDevice, filename.c_str());
				}
			}
		}
		if (FAILED(hr)) {
			MessageBoxA(NULL, "Texture couldn't be loaded", "Error!", MB_ICONERROR | MB_OK);
		}
	}
}

// テクスチャのアルファ有無
bool CAssimpModel::HasAlphaPixels(ID3D11ShaderResourceView* pTexture)
{
/*
	D3DLOCKED_RECT sRect;
	D3DSURFACE_DESC sDesc;
	piTexture->GetLevelDesc(0, &sDesc);
	if (FAILED(piTexture->LockRect(0, &sRect, NULL, D3DLOCK_READONLY)))
	{
		return false;
	}
	const int iPitchDiff = (int)sRect.Pitch - (int)(sDesc.Width * 4);

	struct SColor
	{
		unsigned char b, g, r, a;;
	};
	const SColor* pcData = (const SColor*)sRect.pBits;

	union
	{
		const SColor* pcPointer;
		const unsigned char* pcCharPointer;
	};
	pcPointer = pcData;
	for (unsigned int y = 0; y < sDesc.Height; ++y)
	{
		for (unsigned int x = 0; x < sDesc.Width; ++x)
		{
			if (pcPointer->a != 0xFF)
			{
				piTexture->UnlockRect(0);
				return true;
			}
			pcPointer++;
		}
		pcCharPointer += iPitchDiff;
	}
	piTexture->UnlockRect(0);
*/
// (未サポート)
	return false;
}

// ノード毎に頂点座標の最大最小値をチェック
void CAssimpModel::CalculateBounds(aiNode* piNode, aiVector3D* p_avOut, const aiMatrix4x4& piMatrix)
{
	aiMatrix4x4 mTemp = piNode->mTransformation;
	mTemp.Transpose();
	aiMatrix4x4 aiMe = mTemp * piMatrix;

	for (unsigned int i = 0; i < piNode->mNumMeshes; ++i) {
		for (unsigned int a = 0; a < m_pScene->mMeshes[piNode->mMeshes[i]]->mNumVertices; ++a) {
			aiVector3D pc = m_pScene->mMeshes[piNode->mMeshes[i]]->mVertices[a];

			aiVector3D pc1;
			XMStoreFloat3((XMFLOAT3*)&pc1, XMVector3TransformCoord(XMLoadFloat3((XMFLOAT3*)&pc), XMLoadFloat4x4((XMFLOAT4X4*)&aiMe)));

			p_avOut[0].x = min(p_avOut[0].x, pc1.x);
			p_avOut[0].y = min(p_avOut[0].y, pc1.y);
			p_avOut[0].z = min(p_avOut[0].z, pc1.z);
			p_avOut[1].x = max(p_avOut[1].x, pc1.x);
			p_avOut[1].y = max(p_avOut[1].y, pc1.y);
			p_avOut[1].z = max(p_avOut[1].z, pc1.z);
		}
	}
	for (unsigned int i = 0; i < piNode->mNumChildren; ++i) {
		CalculateBounds(piNode->mChildren[i], p_avOut, aiMe);
	}
}

// 境界ボックス計算
void CAssimpModel::ScaleAsset()
{
	aiVector3D aiVecs[2] = { aiVector3D(1e10f, 1e10f, 1e10f), aiVector3D(-1e10f, -1e10f, -1e10f) };

	if (m_pScene->mRootNode) {
		aiMatrix4x4 m;
		CalculateBounds(m_pScene->mRootNode, aiVecs, m);
	}

	aiVector3D vHalf = (aiVecs[1] - aiVecs[0]) / 2.0f;
	aiVector3D vCenter = aiVecs[0] + vHalf;

	m_vBBox.x = vHalf.x;
	m_vBBox.y = vHalf.y;
	m_vBBox.z = vHalf.z;
	m_vCenter.x = vCenter.x;
	m_vCenter.y = vCenter.y;
	m_vCenter.z = vCenter.z;
}

// アニメーション セット選択
void CAssimpModel::SetAnimIndex(int nAnimIndex)
{
	if (m_pAnimator) {
		m_pAnimator->SetAnimIndex(nAnimIndex);
		this->SetStartAnimTime(0.0f);
	}
}
// 現在アニメーションインデックスを取得
int CAssimpModel::GetCurrentAnimIndex()
{
	if (m_pAnimator) {
		return (int)m_pAnimator->CurrentAnimIndex();
		//m_pAnimator->CurrentAnim();
	}
	return -1;
}

// アニメーション セット数取得
UINT CAssimpModel::GetAnimCount()
{
	if (m_pScene)
		return m_pScene->mNumAnimations;
	//m_pScene->mAnimations[0]->mDuration;
	return 0;
}

double CAssimpModel::GetAnimDuration(int nAnimIndex)
{
	if (m_pScene) {
		if (nAnimIndex < 0) {
			if (m_pAnimator) {
				aiAnimation* pAnim = m_pAnimator->CurrentAnim();
				if (pAnim) {
					return pAnim->mDuration / 60.0;
				}
			}
		}
		if ((UINT)nAnimIndex < m_pScene->mNumAnimations) {
			return m_pScene->mAnimations[nAnimIndex]->mDuration / 60.0;
		}
	}
	return 0.0;
}
