// Assimp���C�u�����p���f���N���X���� [AssimpModel.cpp]
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

// �V�F�[�_�ɓn���l
struct SHADER_GLOBAL {
	XMMATRIX	mWVP;		// ���[���h�~�r���[�~�ˉe�s��(�]�u�s��)
	XMMATRIX	mLitWVP;		// ���[���h�~�r���[�~�ˉe�s��(�]�u�s��)
	XMMATRIX	mW;			// ���[���h�s��(�]�u�s��)
	XMMATRIX	mTex;		// �e�N�X�`���s��(�]�u�s��)
	XMVECTOR	vEye;		// ���_���W
	XMVECTOR	vLightDir;	// ��������
	XMVECTOR	vLa;		// �����F(�A���r�G���g)
	XMVECTOR	vLd;		// �����F(�f�B�t���[�Y)
	XMVECTOR	vLs;		// �����F(�X�y�L����)
};

// �}�e���A�� (�V�F�[�_�p)
struct SHADER_MATERIAL {
	XMVECTOR	vAmbient;	// �A���r�G���g�F
	XMVECTOR	vDiffuse;	// �f�B�t���[�Y�F
	XMVECTOR	vSpecular;	// �X�y�L�����F
	XMVECTOR	vEmissive;	// �G�~�b�V�u�F
	XMVECTOR	vFlags;		// �e�N�X�`���L��
};

// �V�F�[�_�ɓn���{�[���s��z��
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

// �V���h�E�}�b�v�̃V�F�[�_�[
static ID3D11VertexShader*			g_pShadowVertexShader;		// ���_�V�F�[�_
static ID3D11PixelShader*			g_pShadowPixelShader;		// �s�N�Z���V�F�[�_
static ID3D11InputLayout*			g_pShadowInputLayout;		// ���_�t�H�[�}�b�g


// �R���X�g���N�^
AnimEvaluator::AnimEvaluator(const aiAnimation *pAnim)
	: mAnim(pAnim)
	, mLastTime(0.0)
{
	mLastPositions.resize(pAnim->mNumChannels, std::make_tuple(0, 0, 0));
}

// �f�X�g���N�^
AnimEvaluator::~AnimEvaluator()
{
	// (��)
}

// �w�肳�ꂽ�����̃A�j���[�V�����g���b�N��]��
bool AnimEvaluator::Evaluate(double pTime)
{
	// �A�j���[�V�����I���t���O
	bool bExitFlg = false;
	// �L�[�t���[���̍ő�l
	unsigned int nMaxKey = 0;
	for (unsigned int a = 0; a < mAnim->mNumChannels; ++a) 
	{
		const aiNodeAnim* channel = mAnim->mChannels[a];
		// ���W
		if (nMaxKey < channel->mNumPositionKeys)
			nMaxKey = channel->mNumPositionKeys;
		// ��]
		if (nMaxKey < channel->mNumRotationKeys)
			nMaxKey = channel->mNumRotationKeys;
		// �g�k
		if (nMaxKey < channel->mNumScalingKeys)
			nMaxKey = channel->mNumScalingKeys;
	}

	// 1�b������̃e�B�b�N�𒊏o����B�w�肳��Ă��Ȃ��ꍇ�͊���l��z��B
	double ticksPerSecond = mAnim->mTicksPerSecond != 0.0 ? mAnim->mTicksPerSecond : 25.0;
	// �ȍ~�̖���̌v�Z�̓e�B�b�N�P�ʂōs��
	pTime *= ticksPerSecond;

	// �A�j���̎������ԂɊ��蓖��
	double time = 0.0f;
	if (mAnim->mDuration > 0.0) {
		time = fmod(pTime, mAnim->mDuration);
	}

	if (mTransforms.size() != mAnim->mNumChannels) {
		mTransforms.resize(mAnim->mNumChannels);
	}

	// �e�A�j���[�V�����`�����l���̕ϊ��}�g���b�N�X���Z�o
	for (unsigned int a = 0; a < mAnim->mNumChannels; ++a) {
		const aiNodeAnim* channel = mAnim->mChannels[a];

		// ******** ���W ********
		aiVector3D presentPosition(0, 0, 0);
		if (channel->mNumPositionKeys > 0) {
			// ���݂̃t���[���ԍ���T���B���Ԃ��Ō�̎��Ԃ���̏ꍇ�͍Ō�̈ʒu���猟�����A�����łȂ��ꍇ�͍ŏ����猟������B
			// �ŏ�����T�����ϓI�Ȏ�����A�����ƍ����ɏ�������K�v������B
			unsigned int frame = (time >= mLastTime) ? std::get<0>(mLastPositions[a]) : 0;
			while (frame < channel->mNumPositionKeys - 1) {
				if (time < channel->mPositionKeys[frame + 1].mTime) {
					break;
				}
				++frame;
			}

			// �ŏI�t���[��
			if (frame >= nMaxKey - 2)
			{
				bExitFlg = true;
			}

			// ���̃t���[���̒l�Ǝ��̃t���[���̒l�̊Ԃ���
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

		// ******** ��] ********
		aiQuaternion presentRotation(1, 0, 0, 0);
		if (channel->mNumRotationKeys > 0) {
			unsigned int frame = (time >= mLastTime) ? std::get<1>(mLastPositions[a]) : 0;
			while (frame < channel->mNumRotationKeys - 1) {
				if (time < channel->mRotationKeys[frame + 1].mTime) {
					break;
				}
				++frame;
			}

			// �ŏI�t���[��
			if (frame >= nMaxKey - 2)
			{
				bExitFlg = true;
			}

			// ���̃t���[���̒l�Ǝ��̃t���[���̒l�̊Ԃ���
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

		// ******** �g�k ********
		aiVector3D presentScaling(1, 1, 1);
		if (channel->mNumScalingKeys > 0) {
			unsigned int frame = (time >= mLastTime) ? std::get<2>(mLastPositions[a]) : 0;
			while (frame < channel->mNumScalingKeys - 1) {
				if (time < channel->mScalingKeys[frame + 1].mTime) {
					break;
				}
				++frame;
			}

			// �ŏI�t���[��
			if (frame >= nMaxKey - 2)
			{
				bExitFlg = true;
			}

			// TODO: ��Ԃ��K�v�ł�? �����ł͐��`��ԂłȂ��ΐ������B
			presentScaling = channel->mScalingKeys[frame].mValue;
			std::get<2>(mLastPositions[a]) = frame;
		}

		// �ϊ��}�g���b�N�X���\�z
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

// �R���X�g���N�^
SceneAnimator::SceneAnimator(const aiScene* pScene, size_t pAnimIndex)
	: mScene(pScene)
	, mCurrentAnimIndex(-1)
	, mAnimEvaluator(nullptr)
	, mRootNode(nullptr)
{
	// �{�[���̃m�[�h�e�[�u�����쐬
	for (unsigned int i = 0; i < pScene->mNumMeshes; ++i) {
		const aiMesh* mesh = pScene->mMeshes[i];
		for (unsigned int n = 0; n < mesh->mNumBones; ++n) {
			const aiBone* bone = mesh->mBones[n];

			mBoneNodesByName[bone->mName.data] = pScene->mRootNode->FindNode(bone->mName);
		}
	}

	// ���݂̃A�j���[�V������ύX����ƁA���̃A�j���[�V�����̃m�[�h�c���[���쐬�����
	SetAnimIndex(pAnimIndex);

	m_dCurrent = m_dLastPlaying = 0.0;
	m_dAnimSpeed = 1.0f;
	m_dAnimTime = 0.0f;
	m_dLastAnimTime = clock();

	m_bStopAnim = false;
}

// �f�X�g���N�^
SceneAnimator::~SceneAnimator()
{
	SAFE_DELETE(mRootNode);
	SAFE_DELETE(mAnimEvaluator);
}

// �Đ��Ɏg�p����A�j���[�V������ݒ�
void SceneAnimator::SetAnimIndex(size_t pAnimIndex)
{
	// �ύX����?
	if (pAnimIndex == static_cast<unsigned int>(mCurrentAnimIndex)) {
		return;
	}

	// �O��̃A�j���f�[�^���폜
	SAFE_DELETE(mRootNode);
	SAFE_DELETE(mAnimEvaluator);
	mNodesByName.clear();

	mCurrentAnimIndex = static_cast<int>(pAnimIndex);

	// �����m�[�h�c���[�쐬
	// �A�j���[�V�����C���f�b�N�X�������ȏꍇ�ł��A���݂̃V�[����͕킷��悤�ɕϊ��}�g���b�N�X��ݒ�
	mRootNode = CreateNodeTree(mScene->mRootNode, nullptr);

	// �A�j���[�V�����C���f�b�N�X������?
	if (static_cast<unsigned int>(mCurrentAnimIndex) >= mScene->mNumAnimations) {
		return;
	}

	// ���̃A�j���[�V�����̕]���N���X���쐬
	mAnimEvaluator = new AnimEvaluator(mScene->mAnimations[mCurrentAnimIndex]);
}

// �V�[���̃m�[�h�ϊ��}�g���b�N�X���v�Z
bool SceneAnimator::Calculate(double pTime)
{
	// �A�j���[�V������?
	if (!mAnimEvaluator) {
		return false;
	}

	// ���݂̃��[�J���ϊ����v�Z
	bool bExitFlg = mAnimEvaluator->Evaluate(pTime);

	// �S�m�[�h�ϊ��}�g���b�N�X�����ʂōX�V
	UpdateTransforms(mRootNode, mAnimEvaluator->GetTransformations());

	return bExitFlg;
}

// �w�肳�ꂽ�m�[�h�̍ŐV�̃��[�J���ϊ��}�g���b�N�X���擾
const aiMatrix4x4& SceneAnimator::GetLocalTransform(const aiNode* node) const
{
	NodeMap::const_iterator it = mNodesByName.find(node);
	if (it == mNodesByName.end()) {
		return IdentityMatrix;
	}

	return it->second->mLocalTransform;
}

// �w�肳�ꂽ�m�[�h�̍ŐV�̃O���[�o���ϊ��}�g���b�N�X���擾
const aiMatrix4x4& SceneAnimator::GetGlobalTransform(const aiNode* node) const
{
	NodeMap::const_iterator it = mNodesByName.find(node);
	if (it == mNodesByName.end()) {
		return IdentityMatrix;
	}

	return it->second->mGlobalTransform;
}

// ���b�V�����̃{�[���}�g���b�N�X���v�Z
const std::vector<aiMatrix4x4>& SceneAnimator::GetBoneMatrices(const aiNode* pNode, size_t pMeshIndex /* = 0 */)
{
	//ai_assert(pMeshIndex < pNode->mNumMeshes);
	size_t meshIndex = pNode->mMeshes[pMeshIndex];
	//ai_assert(meshIndex < mScene->mNumMeshes);
	const aiMesh* mesh = mScene->mMeshes[meshIndex];

	// �z��̃T�C�Y��ύX���A�P�ʃ}�g���b�N�X�ŏ�����
	mTransforms.resize(mesh->mNumBones, aiMatrix4x4());

	// �O���[�o���ϊ��}�g���b�N�X�̋t�s����v�Z
	aiMatrix4x4 globalInverseMeshTransform = GetGlobalTransform(pNode);
	globalInverseMeshTransform.Inverse();

	// �o�C���h�|�[�Y�̃��b�V�����W����X�L���|�[�Y�̃��b�V�����W�ɕϊ�����{�[���}�g���b�N�X�𐶐�
	// ���� offsetMatrix * currentGlobalTransform * inverseCurrentMeshTransform �ƂȂ�
	for (size_t a = 0; a < mesh->mNumBones; ++a) {
		const aiBone* bone = mesh->mBones[a];
		const aiMatrix4x4& currentGlobalTransform = GetGlobalTransform(mBoneNodesByName[bone->mName.data]);
		mTransforms[a] = globalInverseMeshTransform * currentGlobalTransform * bone->mOffsetMatrix;
	}

	// ���ʂ�Ԃ�
	return mTransforms;
}

// ���݂̃V�[���ƃA�j���[�V�����Ɉ�v��������m�[�h�\�����ċA�I�ɐ���
SceneAnimNode* SceneAnimator::CreateNodeTree(aiNode* pNode, SceneAnimNode* pParent)
{
	// �m�[�h����
	SceneAnimNode* internalNode = new SceneAnimNode(pNode->mName.data);
	internalNode->mParent = pParent;
	mNodesByName[pNode] = internalNode;

	// �m�[�h�̃}�g���b�N�X���R�s�[
	internalNode->mLocalTransform = pNode->mTransformation;
	CalculateGlobalTransform(internalNode);

	// ���̃m�[�h�ɉe����^����A�j���[�V�����g���b�N�̃C���f�b�N�X��T��
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

	// �S�Ă̎q�m�[�h�ɑ΂��āA�쐬���ꂽ�����m�[�h���q�Ƃ��Ċ��蓖��
	for (unsigned int a = 0; a < pNode->mNumChildren; ++a) {
		SceneAnimNode* childNode = CreateNodeTree(pNode->mChildren[a], internalNode);
		internalNode->mChildren.push_back(childNode);
	}

	return internalNode;
}

// �w�肳�ꂽ�}�g���b�N�X�z�񂩂�����m�[�h�ϊ��}�g���b�N�X���ċA�I�ɍX�V
void SceneAnimator::UpdateTransforms(SceneAnimNode* pNode, const std::vector<aiMatrix4x4>& pTransforms)
{
	// �m�[�h�̃��[�J���ϊ��}�g���b�N�X���X�V
	if (pNode->mChannelIndex != -1) {
		//ai_assert(static_cast<unsigned int>(pNode->mChannelIndex) < pTransforms.size());
		pNode->mLocalTransform = pTransforms[pNode->mChannelIndex];
	}

	// �O���[�o���ϊ��}�g���b�N�X���X�V
	CalculateGlobalTransform(pNode);

	// �S�Ă̎q�m�[�h���X�V
	for (std::vector<SceneAnimNode*>::iterator it = pNode->mChildren.begin(); it != pNode->mChildren.end(); ++it) {
		UpdateTransforms(*it, pTransforms);
	}
}

// �w�肳�ꂽ�����m�[�h�̃O���[�o���ϊ��}�g���b�N�X���v�Z
void SceneAnimator::CalculateGlobalTransform(SceneAnimNode* pInternalNode)
{
	// �S�Ă̐e�}�g���b�N�X��A�����āA���̃m�[�h�̃O���[�o���ϊ��}�g���b�N�X���擾
	pInternalNode->mGlobalTransform = pInternalNode->mLocalTransform;
	SceneAnimNode* node = pInternalNode->mParent;
	while (node) {
		pInternalNode->mGlobalTransform = node->mLocalTransform * pInternalNode->mGlobalTransform;
		node = node->mParent;
	}
}

// ���b�V�� �N���X
// �R���X�g���N�^
CAssimpMesh::CAssimpMesh(ID3D11Device *pDevice, CAssimpModel* pModel, vector<TAssimpVertex> aVertex, vector<UINT> aIndex, TAssimpMaterial& material)
	: m_pModel(pModel), m_pVertexBuffer(nullptr), m_pIndexBuffer(nullptr), m_pConstantBuffer0(nullptr), m_pConstantBuffer1(nullptr), m_pConstantBufferBone(nullptr)
{
	m_aVertex = aVertex;
	m_aIndex = aIndex;
	m_material = material;
	SetupMesh(pDevice);
}

// �f�X�g���N�^
CAssimpMesh::~CAssimpMesh()
{
}

// ���
void CAssimpMesh::Release()
{
	SAFE_RELEASE(m_pConstantBuffer1);
	SAFE_RELEASE(m_pConstantBuffer0);
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
	m_material.Release();
}

// ���_�o�b�t�@/�C���f�b�N�X �o�b�t�@����
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

	// �R���X�^���g�o�b�t�@�쐬 �ϊ��s��n���p
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

	// �R���X�^���g�o�b�t�@�쐬 �}�e���A���n���p
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(SHADER_MATERIAL);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;
	hr = pDevice->CreateBuffer(&cb, nullptr, &m_pConstantBuffer1);
	if (FAILED(hr)) {
		return false;
	}

	// �R���X�^���g�o�b�t�@ �{�[���p �쐬
	ZeroMemory(&cb, sizeof(cb));
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(SHADER_BONE);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;
	hr = pDevice->CreateBuffer(&cb, nullptr, &m_pConstantBufferBone);

	return SUCCEEDED(hr);
}

// �{�[�� �}�g���b�N�X�ݒ�
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

// �`��
void CAssimpMesh::Draw(ID3D11DeviceContext* pDC, XMFLOAT4X4& m44World, EByOpacity byOpacity)
{
	// ���[�U��`�}�e���A��
	TAssimpMaterial* pMaterial = m_pModel->GetMaterial();
	if (!pMaterial)
		pMaterial = &m_material;
	// �����x�ɂ��`��ؑ�
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
	// �e�N�X�`���t���O
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
	// �萔�̈�X�V
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

	// ���_�o�b�t�@/�C���f�b�N�X �o�b�t�@���Z�b�g
	UINT stride = sizeof(TAssimpVertex);
	UINT offset = 0;
	pDC->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	pDC->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// �}�e���A���̊e�v�f���V�F�[�_�ɓn��
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

	// �e�N�X�`�����V�F�[�_�ɓn��
	if (pMaterial->pTexture) {
		pDC->PSSetShaderResources(0, 1, &pMaterial->pTexture);
		if (pMaterial->pTexEmmisive)
			pDC->PSSetShaderResources(1, 1, &pMaterial->pTexEmmisive);
		if (pMaterial->pTexTransparent)
			pDC->PSSetShaderResources(2, 1, &pMaterial->pTexTransparent);
		if (pMaterial->pTexSpecular)
			pDC->PSSetShaderResources(3, 1, &pMaterial->pTexSpecular);
	}
	// �V���h�E�}�b�v
	pDC->PSSetShaderResources(4, 1, &g_pShadowResourceView);

	// �{�[�����V�F�[�_�ɓn��
	pDC->VSSetConstantBuffers(2, 1, &m_pConstantBufferBone);

	// �`��
	pDC->DrawIndexed(UINT(m_aIndex.size()), 0, 0);
}

// �`��
void CAssimpMesh::DrawShadow(ID3D11DeviceContext* pDC, XMFLOAT4X4& m44World, EByOpacity byOpacity)
{

	pDC->OMSetRenderTargets(0, nullptr, g_pShadowDepthStencilView);
	pDC->VSSetShader(g_pShadowVertexShader, nullptr, 0);
	pDC->PSSetShader(nullptr, nullptr, 0);
	pDC->IASetInputLayout(g_pShadowInputLayout);

	// ���[�U��`�}�e���A��
	TAssimpMaterial* pMaterial = m_pModel->GetMaterial();
	if (!pMaterial)
		pMaterial = &m_material;
	// �����x�ɂ��`��ؑ�
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
	// �e�N�X�`���t���O
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
	// �萔�̈�X�V
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

	// ���_�o�b�t�@/�C���f�b�N�X �o�b�t�@���Z�b�g
	UINT stride = sizeof(TAssimpVertex);
	UINT offset = 0;
	pDC->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	pDC->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// �}�e���A���̊e�v�f���V�F�[�_�ɓn��
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

	// �e�N�X�`�����V�F�[�_�ɓn��
	if (pMaterial->pTexture) {
		pDC->PSSetShaderResources(0, 1, &pMaterial->pTexture);
		if (pMaterial->pTexEmmisive)
			pDC->PSSetShaderResources(1, 1, &pMaterial->pTexEmmisive);
		if (pMaterial->pTexTransparent)
			pDC->PSSetShaderResources(2, 1, &pMaterial->pTexTransparent);
		if (pMaterial->pTexSpecular)
			pDC->PSSetShaderResources(3, 1, &pMaterial->pTexSpecular);
	}
	// �V���h�E�}�b�v
	pDC->PSSetShaderResources(4, 1, &g_pShadowResourceView);

	// �{�[�����V�F�[�_�ɓn��
	pDC->VSSetConstantBuffers(2, 1, &m_pConstantBufferBone);

	// �`��
	pDC->DrawIndexed(UINT(m_aIndex.size()), 0, 0);
	//// �V���h�E�}�b�v
	//// �e�^�[�Q�b�g�r���[�������_�[�^�[�Q�b�g�ɐݒ�
	//pDC->OMSetRenderTargets(0, nullptr, g_pShadowDepthStencilView);

	//// �V�F�[�_�ݒ�
	//pDC->VSSetShader(g_pShadowVertexShader, nullptr, 0);
	//pDC->PSSetShader(nullptr, nullptr, 0);
	//pDC->IASetInputLayout(g_pShadowInputLayout);

	//// ���_�o�b�t�@���Z�b�g
	//UINT stride = sizeof(VERTEX_3D);
	//UINT offset = 0;
	//pDC->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	//// �C���f�b�N�X�o�b�t�@���Z�b�g
	//pDC->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//// ���[�U��`�}�e���A��
	//TAssimpMaterial* pMaterial = m_pModel->GetMaterial();
	//if (!pMaterial)
	//	pMaterial = &m_material;
	//// �����x�ɂ��`��ؑ�
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
	//// �e�N�X�`���t���O
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

	//// �萔�̈�X�V
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

	//// �{�[�����V�F�[�_�ɓn��
	//pDC->VSSetConstantBuffers(2, 1, &m_pConstantBufferBone);


	//pDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//// �`��
	//pDC->DrawIndexed(UINT(m_aIndex.size()), 0, 0);
}

// ���f�� �N���X
// �ÓI�����o�ϐ�
ID3D11InputLayout* CAssimpModel::m_pVertexLayout;
ID3D11VertexShader* CAssimpModel::m_pVertexShader;
ID3D11PixelShader* CAssimpModel::m_pPixelShader;
ID3D11SamplerState* CAssimpModel::m_pSampleLinear;

//using ModelKey = std::pair<std::string, std::unique_ptr<CAssimpModel>>;


// �R���X�g���N�^
CAssimpModel::CAssimpModel() : m_pMaterial(nullptr), m_pLight(nullptr), m_pCamera(nullptr), m_pAnimator(nullptr), m_pScene(nullptr)
{
	XMStoreFloat4x4(&m_mtxTexture, XMMatrixIdentity());
	XMStoreFloat4x4(&m_mtxWorld, XMMatrixIdentity());
}

// �f�X�g���N�^
CAssimpModel::~CAssimpModel()
{
	if (m_pMaterial)
	{
		delete m_pMaterial;
	}
}

// �e�N�X�`�� �}�g���b�N�X�ݒ�
void CAssimpModel::SetTextureMatrix(DirectX::XMFLOAT4X4& mtxTexture)
{
	m_mtxTexture = mtxTexture;
}

// �e�N�X�`�� �}�g���b�N�X�擾
XMFLOAT4X4& CAssimpModel::GetTextureMatrix()
{
	return m_mtxTexture;
}

// �V�F�[�_������
bool CAssimpModel::InitShader(ID3D11Device* pDevice)
{
	// �V�F�[�_�ǂݍ���
	HRESULT hr = LoadShader("AssimpVertex", "AssimpPixel",
		&m_pVertexShader, &m_pVertexLayout, &m_pPixelShader);
	if (FAILED(hr)) {
		MessageBoxW(0, L"hlsl�ǂݍ��ݎ��s", nullptr, MB_OK);
		return false;
	}

	// �V���h�E�}�b�v�V�F�[�_�[
	hr = LoadShader("AssimpVertex", "AssimpPixel",
		&g_pShadowVertexShader, &g_pShadowInputLayout, &g_pShadowPixelShader);
	if (FAILED(hr)) {
		MessageBoxW(0, L"hlsl�ǂݍ��ݎ��s", nullptr, MB_OK);
		return false;
	}

	// �e�N�X�`���p�T���v���쐬
	CD3D11_DEFAULT def;
	CD3D11_SAMPLER_DESC sd(def);
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	hr = pDevice->CreateSamplerState(&sd, &m_pSampleLinear);
	if (FAILED(hr)) {
		MessageBoxW(0, L"�e�N�X�`���p�T���v���쐬���s", nullptr, MB_OK);
		return false;
	}

	return true;
}

// �V�F�[�_�I������
void CAssimpModel::UninitShader()
{
	SAFE_RELEASE(m_pSampleLinear);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pVertexLayout);
	SAFE_RELEASE(m_pVertexShader);
}

// ���f���ǂݍ���
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

// �A�j���[�V�����̍Đ��ꏊ
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

// �`��
bool CAssimpModel::Draw(ID3D11DeviceContext* pDC, XMFLOAT4X4& mtxWorld, EByOpacity byOpacity, bool bShadowMode)
{
	if (!m_pScene) return false;

	bool bExitFlg = false;

	// �A�j���[�V�����X�V
	if (m_pAnimator && !m_pAnimator->m_bStopAnim) {
		m_pAnimator->m_dCurrent += (m_pAnimator->m_dAnimTime * m_pAnimator->m_dAnimSpeed) / double(CLOCKS_PER_SEC) - m_pAnimator->m_dLastPlaying;
		double time = m_pAnimator->m_dCurrent;

		bExitFlg = m_pAnimator->Calculate(time);
		m_pAnimator->m_dLastPlaying = m_pAnimator->m_dCurrent;

		// ���������Z
		m_pAnimator->m_dAnimTime += clock() - m_pAnimator->m_dLastAnimTime;
		m_pAnimator->m_dLastAnimTime = clock();
	}


	if (bShadowMode)
	{
		// �e�^�[�Q�b�g�r���[�������_�[�^�[�Q�b�g�ɐݒ�
		pDC->OMSetRenderTargets(0, nullptr, g_pShadowDepthStencilView);
	}
	else
	{
		// �����_�[�^�[�Q�b�g
		SetRenderTarget();
	}


	m_mtxWorld = mtxWorld;
	// �g�p����V�F�[�_�[�̓o�^	
	pDC->VSSetShader(m_pVertexShader, nullptr, 0);
	pDC->PSSetShader(m_pPixelShader, nullptr, 0);
	// ���_�C���v�b�g���C�A�E�g���Z�b�g
	pDC->IASetInputLayout(m_pVertexLayout);
	// �v���~�e�B�u�`����Z�b�g
	pDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// �e�N�X�`���T���v�����Z�b�g
	pDC->PSSetSamplers(0, 1, &m_pSampleLinear);
	pDC->PSSetSamplers(1, 1, &g_pShadowSamplerState);
	// �m�[�h�P�ʂŕ`��
	aiMatrix4x4* piMatrix = (aiMatrix4x4*)&m_mtxWorld;
	DrawNode(pDC, m_pScene->mRootNode, *piMatrix, byOpacity, bShadowMode);

	
	return bExitFlg;
}

// �`��
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

		// �{�[���}�g���b�N�X�X�V
		// (�����炭�����Ŏ��s����͕̂s�K�؂����A�������@���s��)
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

	// �S�Ă̎q�m�[�h�������_�����O
	for (UINT i = 0; i < piNode->mNumChildren; ++i)
		DrawNode(pDC, piNode->mChildren[i], piMatrix, byOpacity, bShadowMode);
}

// ���
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

// �S���b�V����荞��
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

// ���b�V���W�J
CAssimpMesh CAssimpModel::processMesh(ID3D11Device* pDevice, aiMesh* mesh)
{
	vector<TAssimpVertex> aVertex;
	vector<UINT> aIndex;
	TAssimpMaterial material;

	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* mat = m_pScene->mMaterials[mesh->mMaterialIndex];

		if (m_textype.empty()) m_textype = determineTextureType(mat);
	}

	// �S���_�̃E�F�C�g�����W(�������̂��߃`�F�b�N��)
	std::vector<std::vector<aiVertexWeight> >weightsPerVertex(mesh->mNumVertices);
	for (unsigned int a = 0; a < mesh->mNumBones; a++) {
		const aiBone* bone = mesh->mBones[a];
		for (unsigned int b = 0; b < bone->mNumWeights; b++)
			weightsPerVertex[bone->mWeights[b].mVertexId].push_back(aiVertexWeight(a, bone->mWeights[b].mWeight));
	}

	// ���b�V���̊e���_���E�H�[�N�X���[
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

		// �{�[���̃C���f�b�N�X�ƃE�F�C�g
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

// �}�e���A���Ǎ�
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

// �e�N�X�`�� �^�C�v����
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

// �e�N�X�`�� �C���f�b�N�X�擾
int CAssimpModel::getTextureIndex(aiString* str)
{
	string tistr;
	tistr = str->C_Str();
	tistr = tistr.substr(1);
	return stoi(tistr);
}

// ���f�� �f�[�^����e�N�X�`���Ǎ�
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

// �e�N�X�`���Ǎ�
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
			else { MessageBoxA(GetMainWnd(), "�e�N�X�`���ǂݍ��݃G���[", filename.c_str(), MB_OK); ; }

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

// �e�N�X�`���̃A���t�@�L��
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
// (���T�|�[�g)
	return false;
}

// �m�[�h���ɒ��_���W�̍ő�ŏ��l���`�F�b�N
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

// ���E�{�b�N�X�v�Z
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

// �A�j���[�V���� �Z�b�g�I��
void CAssimpModel::SetAnimIndex(int nAnimIndex)
{
	if (m_pAnimator) {
		m_pAnimator->SetAnimIndex(nAnimIndex);
		this->SetStartAnimTime(0.0f);
	}
}
// ���݃A�j���[�V�����C���f�b�N�X���擾
int CAssimpModel::GetCurrentAnimIndex()
{
	if (m_pAnimator) {
		return (int)m_pAnimator->CurrentAnimIndex();
		//m_pAnimator->CurrentAnim();
	}
	return -1;
}

// �A�j���[�V���� �Z�b�g���擾
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
