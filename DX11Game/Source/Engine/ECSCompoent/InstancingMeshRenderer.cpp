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
// ���ʃ��b�V���̐���
//
//========================================
HRESULT InstancingMeshRenderer::MakePlane(const std::string tag, int nNumBlockX, int nNumBlockZ, float fSizeBlockX, float fSizeBlockZ,
	float fTexSizeX, float fTexSizeZ)
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

		return S_OK;
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

		return S_OK;
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

		return S_OK;
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


	// �v���~�e�B�u��ʐݒ�
	m_mesh->primitiveType = PT_TRIANGLESTRIP;
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