//==================================================================
//								InstancingMeshRenderer.h
//	���b�V�������_���[
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/23	ECS�p�ɍĐ݌v
//
//===================================================================


//====== �C���N���[�h�� ======
#include "InstancingMeshRenderer.h"
#include "../ECSSystem/RendererSystem.h"
#include "../ECS/Entity/EntityManager.h"

#include "Transform.h"
#include "../Renderer/Camera.h"
#include "../System/Texture.h"

using namespace ECS;

//===== �}�N����` =====

//===== �O���[�o���ϐ� =====
std::map<const std::string, InstancingMesh*, std::less<>> InstancingMeshRenderer::m_meshPool;
std::map<const std::string, std::vector<InstancingMeshData*>, std::less<>> InstancingMeshRenderer::m_meshList;

//========================================
//
//	�R���X�g���N�^
//
//========================================
InstancingMeshRenderer::InstancingMeshRenderer()
{
	m_mesh = nullptr;

	m_data.material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_data.material.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_data.material.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_data.material.Emissive = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_data.material.Power = 50.0f;

	//m_fLayer = 100;
}

//========================================
//
//	�f�X�g���N�^
//
//========================================
InstancingMeshRenderer::~InstancingMeshRenderer()
{

}

//========================================
//
//	������
//
//========================================
void InstancingMeshRenderer::OnCreate()
{
	// �g�����X�t�H�[������擾
	m_transform = m_Parent.lock()->GetComponent<Transform>();

	m_data.mtxWorld = m_transform.lock()->GetWorldMatrix();
	m_data.mtxTexture = &m_mtxTexture;
}

//========================================
//
//	�j����
//
//========================================
void InstancingMeshRenderer::OnDestroy()
{
	//// �V�X�e�����珜�O
	//RendererSystem* sys = GetEntityManager()->GetWorld()->GetSystem<RendererSystem>();
	//if (sys) sys->RemoveList(this);

	// �f�[�^���X�g����폜
	auto itr = m_meshList.find(m_tag);
	if (itr != m_meshList.end())
	{
		auto itr2 = std::find_if(itr->second.begin(), itr->second.end(),
			[this, &itr](InstancingMeshData* p)
			{
				return p == &this->m_data;
			});
		if (itr->second.end() != itr2)
		{
			*itr2 = itr->second.back();
			itr->second.pop_back();
			//itr->second.erase(itr2);
		}
	}
}

//========================================
//
//	�O�`��
//
//========================================
void InstancingMeshRenderer::EarlyDraw(ID3D11DeviceContext* pDC)
{
	//auto itr = m_meshList.find(m_tag);
	//if (m_meshList.end() == itr) return;

	//const auto& mesh = m_mesh;

	//if (mesh)
	//{
	//	DrawInstancingMeshShadow(pDC, mesh, itr->second);
	//}
}

//========================================
//
//	��`��
//
//========================================
void InstancingMeshRenderer::LateDraw(ID3D11DeviceContext* pDC)
{
	auto itr = m_meshList.find(m_tag);
	if (m_meshList.end() == itr) return;


	const auto& mesh = m_mesh;

	if (mesh)
	{
		DrawInstancingMesh(pDC, mesh, itr->second);
	}
}


//========================================
//
// ���b�V���f�[�^�̐���
//
//========================================
bool InstancingMeshRenderer::CreateMeshData(std::string tag)
{
	// �^�O
	m_tag = tag;

	// ���b�V���̌���
	const auto& itr = m_meshPool.find(tag);
	if (m_meshPool.end() != itr)
	{
		m_mesh = itr->second;
		// ���X�g�Ɋi�[
		auto itr2 = m_meshList.find(tag);
		if (itr2 != m_meshList.end())
		{
			itr2->second.push_back(&this->m_data);
		}
		else
		{
			// ���X�g�쐬
			std::vector<InstancingMeshData*> list;
			list.push_back(&this->m_data);
			m_meshList.emplace(tag, list);
		}

		return false;
	}

	// �V�K�쐬
	m_mesh = new InstancingMesh();

	// �v�[���Ɋi�[
	m_meshPool.emplace(tag, m_mesh);
	// ���X�g�쐬
	std::vector<InstancingMeshData*> list;
	list.push_back(&this->m_data);
	m_meshList.emplace(tag, list);

	// �V�X�e���Ɋi�[
	RendererSystem* sys = GetEntityManager()->GetWorld()->GetSystem<RendererSystem>();
	if (sys)
	{
		InstancingMeshRenderer* p = new InstancingMeshRenderer();
		p->m_tag = tag;
		p->m_mesh = m_mesh;
		sys->AddList(p);
	}

	return true;
}

//========================================
//
// ���ʃ��b�V���̐���
//
//========================================
HRESULT InstancingMeshRenderer::MakePlane(const std::string tag, int nNumBlockX, int nNumBlockZ, float fSizeBlockX, float fSizeBlockZ,
	float fTexSizeX, float fTexSizeZ)
{
	// ���b�V���f�[�^�̍쐬
	if (!CreateMeshData(tag)) return S_OK;


	// �v���~�e�B�u��ʐݒ�
	m_mesh->primitiveType = PT_TRIANGLESTRIP;
	// ���_���̐ݒ�
	m_mesh->nNumVertex = (nNumBlockX + 1) * (nNumBlockZ + 1);
	// �C���f�b�N�X���̐ݒ�(�k�ރ|���S���p���l������)
	m_mesh->nNumIndex = (nNumBlockX + 1) * 2 * nNumBlockZ + (nNumBlockZ - 1) * 2;
	// ���_�z��̍쐬
	VERTEX_3D* pVertexWk = new VERTEX_3D[m_mesh->nNumVertex];
	// �C���f�b�N�X�z��̍쐬
	int* pIndexWk = new int[m_mesh->nNumIndex];
	// ���_�z��̒��g�𖄂߂�
	VERTEX_3D* pVtx = pVertexWk;
	for (int z = 0; z < nNumBlockZ + 1; ++z) {
		for (int x = 0; x < nNumBlockX + 1; ++x) {

			// ���_���W�̐ݒ�
			pVtx->vtx.x = x * fSizeBlockX - (nNumBlockX * 0.5f) * fSizeBlockX;
			pVtx->vtx.y = 0.0f;
			pVtx->vtx.z = -z * fSizeBlockZ + (nNumBlockZ * 0.5f) * fSizeBlockZ;
			// �@���̐ݒ�
			pVtx->nor = XMFLOAT3(0.0f, 1.0f, 0.0f);
			// ���ˌ��̐ݒ�
			pVtx->diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			// �e�N�X�`�����W�̐ݒ�
			pVtx->tex.x = fTexSizeX * x;
			pVtx->tex.y = fTexSizeZ * z;
			++pVtx;
		}
	}
	//�C���f�b�N�X�z��̒��g�𖄂߂�
	int* pIdx = pIndexWk;
	for (int z = 0; z < nNumBlockZ; ++z) {
		if (z > 0) {
			// �k�ރ|���S���̂��߂̃_�u��̐ݒ�
			*pIdx++ = (z + 1) * (nNumBlockX + 1);
		}
		for (int x = 0; x < nNumBlockX + 1; ++x) {
			*pIdx++ = (z + 1) * (nNumBlockX + 1) + x;
			*pIdx++ = z * (nNumBlockX + 1) + x;
		}
		if (z < nNumBlockZ - 1) {
			// �k�ރ|���S���̂��߂̃_�u��̐ݒ�
			*pIdx++ = z * (nNumBlockX + 1) + nNumBlockX;
		}
	}

	ID3D11Device* pDevice = GetDevice();
	// ���_�o�b�t�@/�C���f�b�N�X �o�b�t�@����
	HRESULT hr = MakeInstancingMeshVertex(pDevice, m_mesh, pVertexWk, pIndexWk);
	// �ꎞ�z��̉��
	delete[] pVertexWk;
	delete[] pIndexWk;
	return hr;
}

//========================================
//
// �L���[�u���b�V���̐���
//
//========================================
HRESULT InstancingMeshRenderer::MakeCube(const std::string tag)
{
	// ���b�V���f�[�^�̍쐬
	if (!CreateMeshData(tag)) return S_OK;


#define	SIZE_X			(0.5f)											// �����̂̃T�C�Y(X����)
#define	SIZE_Y			(0.5f)											// �����̂̃T�C�Y(Y����)
#define	SIZE_Z			(0.5f)											// �����̂̃T�C�Y(Z����)

#define CUBE_VERTEX		(24)
#define CUBE_INDEX		(36)

	// �v���~�e�B�u�ݒ�
	m_mesh->primitiveType = PT_TRIANGLE;

	VERTEX_3D	vertexWk[CUBE_VERTEX];	// ���_���i�[���[�N
	int			indexWk[CUBE_INDEX];	// �C���f�b�N�X�i�[���[�N

	// ���_��
	m_mesh->nNumVertex = CUBE_VERTEX;

	// ���_���W�̐ݒ�
	// �O
	vertexWk[0].vtx = XMFLOAT3(-SIZE_X, SIZE_Y, -SIZE_Z);
	vertexWk[1].vtx = XMFLOAT3(SIZE_X, SIZE_Y, -SIZE_Z);
	vertexWk[2].vtx = XMFLOAT3(-SIZE_X, -SIZE_Y, -SIZE_Z);
	vertexWk[3].vtx = XMFLOAT3(SIZE_X, -SIZE_Y, -SIZE_Z);
	// �E
	vertexWk[4].vtx = XMFLOAT3(SIZE_X, SIZE_Y, -SIZE_Z);
	vertexWk[5].vtx = XMFLOAT3(SIZE_X, SIZE_Y, SIZE_Z);
	vertexWk[6].vtx = XMFLOAT3(SIZE_X, -SIZE_Y, -SIZE_Z);
	vertexWk[7].vtx = XMFLOAT3(SIZE_X, -SIZE_Y, SIZE_Z);
	// ��
	vertexWk[8].vtx = XMFLOAT3(-SIZE_X, SIZE_Y, SIZE_Z);
	vertexWk[9].vtx = XMFLOAT3(SIZE_X, SIZE_Y, SIZE_Z);
	vertexWk[10].vtx = XMFLOAT3(-SIZE_X, SIZE_Y, -SIZE_Z);
	vertexWk[11].vtx = XMFLOAT3(SIZE_X, SIZE_Y, -SIZE_Z);
	// ��
	vertexWk[12].vtx = XMFLOAT3(SIZE_X, SIZE_Y, SIZE_Z);
	vertexWk[13].vtx = XMFLOAT3(-SIZE_X, SIZE_Y, SIZE_Z);
	vertexWk[14].vtx = XMFLOAT3(SIZE_X, -SIZE_Y, SIZE_Z);
	vertexWk[15].vtx = XMFLOAT3(-SIZE_X, -SIZE_Y, SIZE_Z);
	// ��
	vertexWk[16].vtx = XMFLOAT3(-SIZE_X, SIZE_Y, SIZE_Z);
	vertexWk[17].vtx = XMFLOAT3(-SIZE_X, SIZE_Y, -SIZE_Z);
	vertexWk[18].vtx = XMFLOAT3(-SIZE_X, -SIZE_Y, SIZE_Z);
	vertexWk[19].vtx = XMFLOAT3(-SIZE_X, -SIZE_Y, -SIZE_Z);
	// ��
	vertexWk[20].vtx = XMFLOAT3(SIZE_X, -SIZE_Y, SIZE_Z);
	vertexWk[21].vtx = XMFLOAT3(-SIZE_X, -SIZE_Y, SIZE_Z);
	vertexWk[22].vtx = XMFLOAT3(SIZE_X, -SIZE_Y, -SIZE_Z);
	vertexWk[23].vtx = XMFLOAT3(-SIZE_X, -SIZE_Y, -SIZE_Z);


	// �@���x�N�g���̐ݒ�
	// �O
	vertexWk[0].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertexWk[1].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertexWk[2].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertexWk[3].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	// �E
	vertexWk[4].nor = XMFLOAT3(1.0f, 0.0f, 0.0f);
	vertexWk[5].nor = XMFLOAT3(1.0f, 0.0f, 0.0f);
	vertexWk[6].nor = XMFLOAT3(1.0f, 0.0f, 0.0f);
	vertexWk[7].nor = XMFLOAT3(1.0f, 0.0f, 0.0f);
	// ��
	vertexWk[8].nor = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertexWk[9].nor = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertexWk[10].nor = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertexWk[11].nor = XMFLOAT3(0.0f, 1.0f, 0.0f);
	// ��
	vertexWk[12].nor = XMFLOAT3(0.0f, 0.0f, 1.0f);
	vertexWk[13].nor = XMFLOAT3(0.0f, 0.0f, 1.0f);
	vertexWk[14].nor = XMFLOAT3(0.0f, 0.0f, 1.0f);
	vertexWk[15].nor = XMFLOAT3(0.0f, 0.0f, 1.0f);
	// ��
	vertexWk[16].nor = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	vertexWk[17].nor = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	vertexWk[18].nor = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	vertexWk[19].nor = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	// ��
	vertexWk[20].nor = XMFLOAT3(0.0f, -1.0f, 0.0f);
	vertexWk[21].nor = XMFLOAT3(0.0f, -1.0f, 0.0f);
	vertexWk[22].nor = XMFLOAT3(0.0f, -1.0f, 0.0f);
	vertexWk[23].nor = XMFLOAT3(0.0f, -1.0f, 0.0f);


	// �g�U���ˌ��̐ݒ�
	for (int i = 0; i < CUBE_VERTEX; i++)
	{
		vertexWk[i].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// �e�N�X�`�����W�̐ݒ�
	for (int i = 0; i < CUBE_VERTEX; i += 4)
	{
		vertexWk[0 + i].tex = XMFLOAT2(0.0f, 0.0f);
		vertexWk[1 + i].tex = XMFLOAT2(1.0f, 0.0f);
		vertexWk[2 + i].tex = XMFLOAT2(0.0f, 1.0f);
		vertexWk[3 + i].tex = XMFLOAT2(1.0f, 1.0f);
	}

	// �C���f�b�N�X��
	m_mesh->nNumIndex = CUBE_INDEX;

	// �C���f�b�N�X�z��̐ݒ�
	for (int i = 0, j = 0; i < CUBE_INDEX; i += 6, j += 4)
	{
		indexWk[0 + i] = 0 + j;
		indexWk[1 + i] = 1 + j;
		indexWk[2 + i] = 2 + j;
		indexWk[3 + i] = 2 + j;
		indexWk[4 + i] = 1 + j;
		indexWk[5 + i] = 3 + j;
	}

	ID3D11Device* pDevice = GetDevice();
	return MakeInstancingMeshVertex(pDevice, m_mesh, vertexWk, indexWk);
	return S_OK;
}

//========================================
//
// �X�t�B�A���b�V���̐���
//
//========================================
HRESULT InstancingMeshRenderer::MakeSphere(std::string tag, int nSegment, float fTexSplit, XMFLOAT3 pos)
{
	// ���b�V���f�[�^�̍쐬
	if (!CreateMeshData(tag)) return S_OK;


	// �v���~�e�B�u��ʐݒ�
	m_mesh->primitiveType = PT_TRIANGLE;
	float fScale = 0.5f;

	//���_�o�b�t�@�쐬
	m_mesh->nNumVertex = (nSegment + 1) * (nSegment / 2 + 1);
	VERTEX_3D* pVertexWk = new VERTEX_3D[m_mesh->nNumVertex];

	for (int i = 0; i <= (nSegment / 2); ++i) {
		float irad = XM_PI * 2.0f / (float)nSegment * (float)i;
		float y = (float)cos(irad);
		float r = (float)sin(irad);
		float v = (float)i / (float)(nSegment / 2) * fTexSplit;
		for (int j = 0; j <= nSegment; ++j) {
			float jrad = XM_PI * 2.0f / (float)nSegment * (float)j;
			float x = r * (float)cos(jrad);
			float z = r * (float)sin(jrad);
			float u = (float)j / (float)nSegment * fTexSplit;
			int   inx = i * (nSegment + 1) + j;
			pVertexWk[inx].vtx.x = x * fScale + pos.x;
			pVertexWk[inx].vtx.y = y * fScale + pos.y;
			pVertexWk[inx].vtx.z = z * fScale + pos.z;
			pVertexWk[inx].nor.x = x;
			pVertexWk[inx].nor.y = y;
			pVertexWk[inx].nor.z = z;
			pVertexWk[inx].tex.x = u;
			pVertexWk[inx].tex.y = v;
			pVertexWk[inx].diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
		}
	}

	// �C���f�b�N�X�o�b�t�@�̍쐬
	m_mesh->nNumIndex = nSegment * 3 + nSegment * (nSegment / 2 - 1) * 6 + nSegment * 3;
	int* pIndexWk = new int[m_mesh->nNumIndex];

	int icount = 0;
	int i = 0;
	for (int j = 0; j < nSegment; ++j) {
		pIndexWk[icount] = i * (nSegment + 1) + j;
		pIndexWk[icount + 1] = (i + 1) * (nSegment + 1) + j + 1;
		pIndexWk[icount + 2] = (i + 1) * (nSegment + 1) + j;
		icount += 3;
	}
	for (i = 1; i < nSegment / 2; ++i) {
		for (int j = 0; j < nSegment; ++j) {
			pIndexWk[icount] = i * (nSegment + 1) + j;
			pIndexWk[icount + 1] = i * (nSegment + 1) + j + 1;
			pIndexWk[icount + 2] = (i + 1) * (nSegment + 1) + j;
			icount += 3;
			pIndexWk[icount] = i * (nSegment + 1) + j + 1;
			pIndexWk[icount + 1] = (i + 1) * (nSegment + 1) + j + 1;
			pIndexWk[icount + 2] = (i + 1) * (nSegment + 1) + j;
			icount += 3;
		}
	}
	i = nSegment / 2;
	for (int j = 0; j < nSegment; ++j) {
		pIndexWk[icount] = i * (nSegment + 1) + j;
		pIndexWk[icount + 1] = (i + 1) * (nSegment + 1) + j + 1;
		pIndexWk[icount + 2] = (i + 1) * (nSegment + 1) + j;
		icount += 3;
	}

	// ���_�o�b�t�@/�C���f�b�N�X �o�b�t�@����
	ID3D11Device* pDevice = GetDevice();
	HRESULT hr = MakeInstancingMeshVertex(pDevice, m_mesh, pVertexWk, pIndexWk);
	// �ꎞ�z��̉��
	delete[] pVertexWk;
	delete[] pIndexWk;
	return hr;

}


HRESULT InstancingMeshRenderer::MakeSphere2(std::string tag,
	int nNumBlockX, int nNumBlockY, float fSize,
	float fTexSizeX, float fTexSizeY,
	float fPosX, float fPosY, float fPosZ)
{
	// ���b�V���f�[�^�̍쐬
	if (!CreateMeshData(tag)) return S_OK;


	// �v���~�e�B�u��ʐݒ�
	m_mesh->primitiveType = PT_TRIANGLESTRIP;
	// ���_���̐ݒ�
	m_mesh->nNumVertex = (nNumBlockX + 1) * (nNumBlockY + 1);
	// �C���f�b�N�X���̐ݒ�(�k�ރ|���S���p���l������)
	m_mesh->nNumIndex = (nNumBlockX + 1) * 2 * nNumBlockY + (nNumBlockY - 1) * 2;
	// ���_�z��̍쐬
	VERTEX_3D* pVertexWk = new VERTEX_3D[m_mesh->nNumVertex];
	// �C���f�b�N�X�z��̍쐬
	int* pIndexWk = new int[m_mesh->nNumIndex];
	// ���_�z��̒��g�𖄂߂�
	VERTEX_3D* pVtx = pVertexWk;

	for (int y = 0; y < nNumBlockY + 1; ++y) {
		for (int x = 0; x < nNumBlockX + 1; ++x) {

			// ���_���W�̐ݒ�
			pVtx->vtx.x = 0.0f;
			pVtx->vtx.y = 1.0f;
			pVtx->vtx.z = 0.0f;
			// �p�x�ɑ΂����]�}�g���b�N�X�����߂�
			XMMATRIX mR = XMMatrixRotationX(XMConvertToRadians(-x * (-180.0f / nNumBlockX)));
			mR *= XMMatrixRotationY(XMConvertToRadians(-y * (360.0f / nNumBlockY)));
			// ���W����]�}�g���b�N�X�ŉ�]������
			XMVECTOR v = XMLoadFloat3(&pVtx->vtx);
			v = XMVector3TransformCoord(v, mR);
			v = XMVector3Normalize(v);
			XMStoreFloat3(&pVtx->vtx, v);

			// �@���̐ݒ�
			pVtx->nor = pVtx->vtx;

			// �傫��
			pVtx->vtx.x *= fSize;
			pVtx->vtx.y *= fSize;
			pVtx->vtx.z *= fSize;

			// �ʒu
			pVtx->vtx.x += fPosX;
			pVtx->vtx.y += fPosY;
			pVtx->vtx.z += fPosZ;

			// ���ˌ��̐ݒ�
			pVtx->diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			// �e�N�X�`�����W�̐ݒ�
			pVtx->tex.x = fTexSizeX * x;
			pVtx->tex.y = fTexSizeY * y;
			++pVtx;
		}
	}
	//�C���f�b�N�X�z��̒��g�𖄂߂�
	int* pIdx = pIndexWk;
	for (int z = 0; z < nNumBlockY; ++z) {
		if (z > 0) {
			// �k�ރ|���S���̂��߂̃_�u��̐ݒ�
			*pIdx++ = (z + 1) * (nNumBlockX + 1);
		}
		for (int x = 0; x < nNumBlockX + 1; ++x) {
			*pIdx++ = (z + 1) * (nNumBlockX + 1) + x;
			*pIdx++ = z * (nNumBlockX + 1) + x;
		}
		if (z < nNumBlockY - 1) {
			// �k�ރ|���S���̂��߂̃_�u��̐ݒ�
			*pIdx++ = z * (nNumBlockX + 1) + nNumBlockX;
		}
	}

	ID3D11Device* pDevice = GetDevice();
	// ���_�o�b�t�@/�C���f�b�N�X �o�b�t�@����
	HRESULT hr = MakeInstancingMeshVertex(pDevice, m_mesh, pVertexWk, pIndexWk);
	// �ꎞ�z��̉��
	delete[] pVertexWk;
	delete[] pIndexWk;
	return hr;
}

//========================================
//
// �l�ʑ� Tetraheron
//
//========================================
HRESULT InstancingMeshRenderer::MakeTetraheron(const std::string tag)
{
	// ���b�V���f�[�^�̍쐬
	if (!CreateMeshData(tag)) return S_OK;

	// �v���~�e�B�u�ݒ�
	m_mesh->primitiveType = PT_TRIANGLE;

	// ���_��
	const int nVertexNum = 3 * 4;
	const int nIndexNum = 3 * 4;

	VERTEX_3D	vertexWk[nVertexNum];	// ���_���i�[���[�N
	int			indexWk[nIndexNum];	// �C���f�b�N�X�i�[���[�N

	// �g���܂킷���_���W
	XMFLOAT3 ver[4] = {
		XMFLOAT3(-0.81649658, - 0.47140452,   0.33333333),
		XMFLOAT3( 0.81649658, - 0.47140452,   0.33333333),
		XMFLOAT3( 0.00000000,   0.00000000, - 1.00000000),
		XMFLOAT3( 0.00000000,   0.94280904,   0.33333333),
	};

	// �g���܂킷�C���f�b�N�X
	int index[nIndexNum] = {
		1, 2, 4,
		1, 4, 3,
		1, 3, 2,
		2, 3, 4,
	};

	// ���_��
	m_mesh->nNumVertex = nVertexNum;

	// ���_���W�̐ݒ�
	for (int i = 0; i < nVertexNum; ++i)
	{
		Vector3 halfV = ver[index[i] - 1];
		halfV /= 2;
		vertexWk[i].vtx = *halfV.GetFloat3();
	}

	// �@���x�N�g���̐ݒ�
	for (int i = 0; i < nVertexNum; i += 3)
	{
		XMFLOAT3 n = *Vector3::Cross(ver[index[i + 0] - 1], ver[index[i + 1] - 1], ver[index[i + 2] - 1]).GetFloat3();
		vertexWk[i + 0].nor = n;
		vertexWk[i + 1].nor = n;
		vertexWk[i + 2].nor = n;
	}

	// �g�U���ˌ��̐ݒ�
	for (int i = 0; i < nVertexNum; i++)
	{
		vertexWk[i].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// �e�N�X�`�����W�̐ݒ�
	for (int i = 0; i < nVertexNum; i += 4)
	{
		vertexWk[0 + i].tex = XMFLOAT2(0.0f, 0.0f);
		vertexWk[1 + i].tex = XMFLOAT2(1.0f, 0.0f);
		vertexWk[2 + i].tex = XMFLOAT2(0.0f, 1.0f);
		vertexWk[3 + i].tex = XMFLOAT2(1.0f, 1.0f);
	}

	// �C���f�b�N�X��
	m_mesh->nNumIndex = nIndexNum;

	// �C���f�b�N�X�z��̐ݒ�
	for (int i = 0; i < nIndexNum; ++i)
	{
		indexWk[i] = i;
	}

	ID3D11Device* pDevice = GetDevice();
	return MakeInstancingMeshVertex(pDevice, m_mesh, vertexWk, indexWk);
}

//========================================
//
// ���ʑ� Octahedron
//
//========================================
HRESULT InstancingMeshRenderer::MakeOctahedron(const std::string tag)
{
	// ���b�V���f�[�^�̍쐬
	if (!CreateMeshData(tag)) return S_OK;

	// �v���~�e�B�u�ݒ�
	m_mesh->primitiveType = PT_TRIANGLE;

	// ���_��
	const int nVertexNum = 3 * 8;
	const int nIndexNum  = 3 * 8;

	VERTEX_3D	vertexWk[nVertexNum];	// ���_���i�[���[�N
	int			indexWk[nIndexNum];	// �C���f�b�N�X�i�[���[�N

	// �g���܂킷���_���W
	XMFLOAT3 ver[6] = {
		XMFLOAT3(-0.70710678, - 0.70710678,  0.00000000),
		XMFLOAT3(-0.70710678,  0.70710678,  0.00000000),
		XMFLOAT3(0.70710678,  0.70710678,  0.00000000),
		XMFLOAT3(0.70710678, - 0.70710678,  0.00000000),
		XMFLOAT3(0.00000000,  0.00000000, - 1.00000000),
		XMFLOAT3(0.00000000,  0.00000000,  1.00000000),
	};

	// �g���܂킷�C���f�b�N�X
	int index[nIndexNum] = {
		1, 2, 5,
		1, 5, 4,
		1, 4, 6,
		1, 6, 2,
		2, 3, 5,
		2, 6, 3,
		3, 4, 5,
		3, 6, 4,
	};

	// ���_��
	m_mesh->nNumVertex = nVertexNum;

	// ���_���W�̐ݒ�
	for (int i = 0; i < nVertexNum; ++i)
	{
		Vector3 halfV = ver[index[i] - 1];
		halfV /= 2;
		vertexWk[i].vtx = *halfV.GetFloat3();
	}

	// �@���x�N�g���̐ݒ�
	for (int i = 0; i < nVertexNum; i += 3)
	{
		XMFLOAT3 n = *Vector3::Cross(ver[index[i + 0] - 1], ver[index[i + 1] - 1], ver[index[i + 2] - 1]).GetFloat3();
		vertexWk[i + 0].nor = n;
		vertexWk[i + 1].nor = n;
		vertexWk[i + 2].nor = n;
	}

	// �g�U���ˌ��̐ݒ�
	for (int i = 0; i < nVertexNum; i++)
	{
		vertexWk[i].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// �e�N�X�`�����W�̐ݒ�
	for (int i = 0; i < nVertexNum; i += 4)
	{
		vertexWk[0 + i].tex = XMFLOAT2(0.0f, 0.0f);
		vertexWk[1 + i].tex = XMFLOAT2(1.0f, 0.0f);
		vertexWk[2 + i].tex = XMFLOAT2(0.0f, 1.0f);
		vertexWk[3 + i].tex = XMFLOAT2(1.0f, 1.0f);
	}

	// �C���f�b�N�X��
	m_mesh->nNumIndex = nIndexNum;

	// �C���f�b�N�X�z��̐ݒ�
	for (int i = 0; i < nIndexNum; ++i)
	{
		indexWk[i] = i;
	}

	ID3D11Device* pDevice = GetDevice();
	return MakeInstancingMeshVertex(pDevice, m_mesh, vertexWk, indexWk);
}

//========================================
//
// �\��ʑ�
//
//========================================
HRESULT InstancingMeshRenderer::MakeDodecahedron(const std::string tag)
{
	// ���b�V���f�[�^�̍쐬
	if (!CreateMeshData(tag)) return S_OK;

	// �v���~�e�B�u�ݒ�
	m_mesh->primitiveType = PT_TRIANGLE;

	// ���_��
	const int nVertexNum = 3 * 36;
	const int nIndexNum = 3 * 36;

	VERTEX_3D	vertexWk[nVertexNum];	// ���_���i�[���[�N
	int			indexWk[nIndexNum];	// �C���f�b�N�X�i�[���[�N

	// �g���܂킷���_���W
	XMFLOAT3 ver[20] = {
		XMFLOAT3(-0.356822, - 0.491124,   0.794654),
		XMFLOAT3( 0.356822, - 0.491124,   0.794654),
		XMFLOAT3( 0.577350,   0.187592,   0.794655),
		XMFLOAT3( 0.000000,   0.607062,   0.794655),
		XMFLOAT3(-0.577350,   0.187592,   0.794655),
		XMFLOAT3(-0.934172,   0.303531,   0.187593),
		XMFLOAT3(-0.934172, - 0.303531, - 0.187593),
		XMFLOAT3(-0.577350, - 0.794655,   0.187592),
		XMFLOAT3( 0.000000, - 0.982247, - 0.187593),
		XMFLOAT3( 0.577350, - 0.794655,   0.187592),
		XMFLOAT3( 0.934172, - 0.303531, - 0.187593),
		XMFLOAT3( 0.934172,   0.303531,   0.187593),
		XMFLOAT3( 0.000000, - 0.607062, - 0.794655),
		XMFLOAT3( 0.577350, - 0.187592, - 0.794655),
		XMFLOAT3(-0.577350, - 0.187592, - 0.794655),
		XMFLOAT3( 0.000000,   0.982247,   0.187593),
		XMFLOAT3(-0.577350,   0.794655, - 0.187592),
		XMFLOAT3( 0.577350,   0.794655, - 0.187592),
		XMFLOAT3( 0.356822,   0.491124, - 0.794654),
		XMFLOAT3(-0.356822,   0.491124, - 0.794654),
	};

	// �g���܂킷�C���f�b�N�X
	int index[nIndexNum] = {
		2, 4, 5,
		6, 7, 8,
		8, 10, 2,
		10, 11, 12,
		9, 14, 11,
		8, 7, 15,
		16, 17, 6,
		12, 16, 4,
		14, 19, 18,
		15, 19, 14,
		17, 20, 15,
		16, 18, 19,
		5, 1, 2,
		2, 3, 4,
		8, 1, 5,
		5, 6, 8,
		2, 1, 8,
		8, 9, 10,
		3, 2, 12,
		2, 10, 12,
		11, 10, 9,
		9, 13, 14,
		13, 9, 15,
		9, 8, 15,
		6, 5, 4,
		4, 16, 6,
		4, 3, 12,
		12, 18, 16,
		12, 11, 18,
		11, 14, 18,
		14, 13, 15,
		15, 20, 19,
		15, 7, 6,
		6, 17, 15,
		20, 17, 19,
		17, 16, 19,
	};

	// ���_��
	m_mesh->nNumVertex = nVertexNum;

	// ���_���W�̐ݒ�
	for (int i = 0; i < nVertexNum; ++i)
	{
		Vector3 halfV = ver[index[i] - 1];
		halfV /= 2;
		vertexWk[i].vtx = *halfV.GetFloat3();
	}

	// �@���x�N�g���̐ݒ�
	for (int i = 0; i < nVertexNum; i += 3)
	{
		XMFLOAT3 n = *Vector3::Cross(ver[index[i + 0] - 1], ver[index[i + 1] - 1], ver[index[i + 2] - 1]).GetFloat3();
		vertexWk[i + 0].nor = n;
		vertexWk[i + 1].nor = n;
		vertexWk[i + 2].nor = n;
	}

	// �g�U���ˌ��̐ݒ�
	for (int i = 0; i < nVertexNum; i++)
	{
		vertexWk[i].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// �e�N�X�`�����W�̐ݒ�
	for (int i = 0; i < nVertexNum; i += 4)
	{
		vertexWk[0 + i].tex = XMFLOAT2(0.0f, 0.0f);
		vertexWk[1 + i].tex = XMFLOAT2(1.0f, 0.0f);
		vertexWk[2 + i].tex = XMFLOAT2(0.0f, 1.0f);
		vertexWk[3 + i].tex = XMFLOAT2(1.0f, 1.0f);
	}

	// �C���f�b�N�X��
	m_mesh->nNumIndex = nIndexNum;

	// �C���f�b�N�X�z��̐ݒ�
	for (int i = 0; i < nIndexNum; ++i)
	{
		indexWk[i] = i;
	}

	ID3D11Device* pDevice = GetDevice();
	return MakeInstancingMeshVertex(pDevice, m_mesh, vertexWk, indexWk);
}

//========================================
//
// ��\�ʑ�
//
//========================================
HRESULT InstancingMeshRenderer::MakeIcosahedron(const std::string tag)
{
	// ���b�V���f�[�^�̍쐬
	if (!CreateMeshData(tag)) return S_OK;

	// �v���~�e�B�u�ݒ�
	m_mesh->primitiveType = PT_TRIANGLE;

	// ���_��
	const int nVertexNum = 3 * 20;
	const int nIndexNum = 3 * 20;

	VERTEX_3D	vertexWk[nVertexNum];	// ���_���i�[���[�N
	int			indexWk[nIndexNum];	// �C���f�b�N�X�i�[���[�N

	// �g���܂킷���_���W
	XMFLOAT3 ver[12] = {
		XMFLOAT3(-0.52573111, - 0.72360680,   0.44721360),
		XMFLOAT3(-0.85065081,   0.27639320,   0.44721360),
		XMFLOAT3(-0.00000000,   0.89442719,   0.44721360),
		XMFLOAT3( 0.85065081,   0.27639320,   0.44721360),
		XMFLOAT3( 0.52573111, - 0.72360680,   0.44721360),
		XMFLOAT3( 0.00000000, - 0.89442719, - 0.44721360),
		XMFLOAT3(-0.85065081, - 0.27639320, - 0.44721360),
		XMFLOAT3(-0.52573111,   0.72360680, - 0.44721360),
		XMFLOAT3( 0.52573111,   0.72360680, - 0.44721360),
		XMFLOAT3( 0.85065081, - 0.27639320, - 0.44721360),
		XMFLOAT3( 0.00000000,   0.00000000,   1.00000000),
		XMFLOAT3(-0.00000000,   0.00000000, - 1.00000000),
	};

	// �g���܂킷�C���f�b�N�X
	int index[nIndexNum] = {
		1, 2 ,7  ,
		1, 7 ,6	 ,
		1, 6 ,5	 ,
		1, 5 ,11 ,
		1, 11, 2 ,
		2, 3 ,8	 ,
		2, 8 ,7	 ,
		2, 11, 3 ,
		3, 4 ,9	 ,
		3, 9 ,8	 ,
		3, 11, 4 ,
		4, 5 ,10 ,
		4, 10, 9 ,
		4, 11, 5 ,
		5, 6 ,10 ,
		6, 7 ,12 ,
		6, 12, 10,
		7, 8 ,12 ,
		8, 9 ,12 ,
		9, 10, 12,
	};

	// ���_��
	m_mesh->nNumVertex = nVertexNum;

	// ���_���W�̐ݒ�
	for (int i = 0; i < nVertexNum; ++i)
	{
		Vector3 halfV = ver[index[i] - 1];
		halfV /= 2;
		vertexWk[i].vtx = *halfV.GetFloat3();
	}

	// �@���x�N�g���̐ݒ�
	for (int i = 0; i < nVertexNum; i += 3)
	{
		XMFLOAT3 n = *Vector3::Cross(ver[index[i + 0] - 1], ver[index[i + 1] - 1], ver[index[i + 2] - 1]).GetFloat3();
		vertexWk[i + 0].nor = n;
		vertexWk[i + 1].nor = n;
		vertexWk[i + 2].nor = n;
	}

	// �g�U���ˌ��̐ݒ�
	for (int i = 0; i < nVertexNum; i++)
	{
		vertexWk[i].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// �e�N�X�`�����W�̐ݒ�
	for (int i = 0; i < nVertexNum; i += 4)
	{
		vertexWk[0 + i].tex = XMFLOAT2(0.0f, 0.0f);
		vertexWk[1 + i].tex = XMFLOAT2(1.0f, 0.0f);
		vertexWk[2 + i].tex = XMFLOAT2(0.0f, 1.0f);
		vertexWk[3 + i].tex = XMFLOAT2(1.0f, 1.0f);
	}

	// �C���f�b�N�X��
	m_mesh->nNumIndex = nIndexNum;

	// �C���f�b�N�X�z��̐ݒ�
	for (int i = 0; i < nIndexNum; ++i)
	{
		indexWk[i] = i;
	}

	ID3D11Device* pDevice = GetDevice();
	return MakeInstancingMeshVertex(pDevice, m_mesh, vertexWk, indexWk);
}

//========================================
//
// �x�[�X�J���[�e�N�X�`���̃Z�b�g
//
//========================================
void InstancingMeshRenderer::SetDiffuseTexture(const char* filename)
{
	// ���b�V���擾
	auto pInstancingMesh = m_mesh;
	// �e�N�X�`��
	if (pInstancingMesh)
		pInstancingMesh->pTexture = CTexture::GetTexture(GetDevice(), filename);
}

//========================================
//
// �x�[�X�J���[�e�N�X�`���̃Z�b�g
//
//========================================
void InstancingMeshRenderer::SetDiffuseTexture(const wchar_t* filename)
{
	// ���b�V���擾
	auto pInstancingMesh = m_mesh;
	// �e�N�X�`��
	if (pInstancingMesh)
		pInstancingMesh->pTexture = CTexture::GetTexture(GetDevice(), filename);
}