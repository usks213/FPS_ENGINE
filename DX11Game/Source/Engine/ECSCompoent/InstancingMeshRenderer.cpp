//==================================================================
//								InstancingMeshRenderer.h
//	メッシュレンダラー
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/23	ECS用に再設計
//
//===================================================================


//====== インクルード部 ======
#include "InstancingMeshRenderer.h"
#include "../ECSSystem/RendererSystem.h"
#include "../ECS/Entity/EntityManager.h"

#include "../ECSEntity/GameObject.h"
#include "Transform.h"
#include "../Renderer/Camera.h"
#include "../System/Texture.h"
#include "../System/debugproc.h"


using namespace ECS;

//===== マクロ定義 =====

//===== グローバル変数 =====
std::map<const std::string, InstancingMesh*, std::less<>> InstancingMeshRenderer::m_meshPool;
std::map<const std::string, std::vector<InstancingMeshData*>, std::less<>> InstancingMeshRenderer::m_meshList;

//========================================
//
//	コンストラクタ
//
//========================================
InstancingMeshRenderer::InstancingMeshRenderer()
{
	m_mesh = nullptr;
	m_fLayer = VIEW_FAR_Z - 100;
}

//========================================
//
//	デストラクタ
//
//========================================
InstancingMeshRenderer::~InstancingMeshRenderer()
{

}

//========================================
//
//	生成時
//
//========================================
void InstancingMeshRenderer::OnCreate()
{
	// トランスフォームから取得
	m_transform = m_Parent.lock()->GetComponent<Transform>();

	// インスタンスシングデータの初期化
	m_data.mtxWorld = m_transform.lock()->GetWorldMatrix();
	m_data.pPos = m_transform.lock()->m_pos.GetFloat3();
	m_data.pRot = m_transform.lock()->m_rot.GetFloat3();
	m_data.pScale = m_transform.lock()->m_scale.GetFloat3();
}

//========================================
//
//	破棄時
//
//========================================
void InstancingMeshRenderer::OnDestroy()
{
	//// システムから除外
	//RendererSystem* sys = GetEntityManager()->GetWorld()->GetSystem<RendererSystem>();
	//if (sys) sys->RemoveList(this);

	// データリストから削除
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
//	前描画
//
//========================================
void InstancingMeshRenderer::EarlyDraw(ID3D11DeviceContext* pDC)
{
	auto itr = m_meshList.find(m_tag);
	if (m_meshList.end() == itr) return;

	const auto& mesh = m_mesh;

	if (mesh)
	{
		DrawInstancingMeshShadow(pDC, mesh, itr->second);
	}
}

//========================================
//
//	後描画
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
// メッシュデータの生成
//
//========================================
bool InstancingMeshRenderer::CreateMeshData(std::string tag)
{
	// タグ
	m_tag = tag;

	// メッシュの検索
	const auto& itr = m_meshPool.find(tag);
	if (m_meshPool.end() != itr)
	{
		// あった
		m_mesh = itr->second;

		// メッシュデータがない→
		if (m_meshList[tag].size() <= 0)
		{
			// システムに格納
			RendererSystem* sys = GetEntityManager()->GetWorld()->GetSystem<RendererSystem>();
			if (sys)
			{
				const auto& obj = GetEntityManager()->CreateEntity<GameObject>();
				const auto& p = obj->AddComponent<InstancingMeshRenderer>();
				p->m_tag = tag;
				p->m_mesh = m_mesh;
				p->m_mesh->mtxTexture = &p->m_mtxTexture;
				sys->AddList(p.get());
				// メッシュリストに
				m_meshList[tag].push_back(&p->m_data);
			}
		}

		// リストがある時
		auto itr2 = m_meshList.find(tag);
		if (itr2 != m_meshList.end())
		{
			itr2->second.push_back(&this->m_data);
		}
		else
		{
			// リスト作成
			std::vector<InstancingMeshData*> list;
			list.push_back(&this->m_data);
			m_meshList.emplace(tag, list);
		}

		return false;
	}

	// リスト作成
	std::vector<InstancingMeshData*> list;
	list.push_back(&this->m_data);
	m_meshList.emplace(tag, list);

	// 新規作成
	m_mesh = new InstancingMesh();
	// プールに格納
	m_meshPool.emplace(tag, m_mesh);


	// システムに格納
	RendererSystem* sys = GetEntityManager()->GetWorld()->GetSystem<RendererSystem>();
	if (sys)
	{
		const auto& obj = GetEntityManager()->CreateEntity<GameObject>();
		const auto& p = obj->AddComponent<InstancingMeshRenderer>();
		p->m_tag = tag;
		p->m_mesh = m_mesh;
		p->m_mesh->mtxTexture = &p->m_mtxTexture;
		sys->AddList(p.get());
		// メッシュリストに
		m_meshList[tag].push_back(&p->m_data);
	}

	return true;
}

//========================================
//
// スプライト
//
//========================================
HRESULT InstancingMeshRenderer::MakeSprite(const std::string tag)
{
	// メッシュデータの作成
	if (!CreateMeshData(tag)) return S_OK;

	// オブジェクトの頂点配列を生成
	m_mesh->nNumVertex = 4;
	VERTEX_3D* pVertexWk = new VERTEX_3D[m_mesh->nNumVertex];

	// 頂点配列の中身を埋める
	VERTEX_3D* pVtx = pVertexWk;

	// 頂点座標の設定
	pVtx[0].vtx = XMFLOAT3(-1.0f / 2, -1.0f / 2, 0.0f);
	pVtx[1].vtx = XMFLOAT3(-1.0f / 2, 1.0f / 2, 0.0f);
	pVtx[2].vtx = XMFLOAT3(1.0f / 2, -1.0f / 2, 0.0f);
	pVtx[3].vtx = XMFLOAT3(1.0f / 2, 1.0f / 2, 0.0f);

	// 法線の設定
	pVtx[0].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVtx[1].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVtx[2].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVtx[3].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// 反射光の設定
	pVtx[0].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	pVtx[0].tex = XMFLOAT2(0.0f, 1.0f);
	pVtx[1].tex = XMFLOAT2(0.0f, 0.0f);
	pVtx[2].tex = XMFLOAT2(1.0f, 1.0f);
	pVtx[3].tex = XMFLOAT2(1.0f, 0.0f);

	// インデックス配列を生成
	m_mesh->nNumIndex = 4;
	int* pIndexWk = new int[m_mesh->nNumIndex];

	// インデックス配列の中身を埋める
	pIndexWk[0] = 0;
	pIndexWk[1] = 1;
	pIndexWk[2] = 2;
	pIndexWk[3] = 3;

	ID3D11Device* pDevice = GetDevice();
	// 頂点バッファ生成
	HRESULT hr = MakeInstancingMeshVertex(pDevice, m_mesh, pVertexWk, pIndexWk);

	// 一時配列の解放
	delete[] pIndexWk;
	delete[] pVertexWk;

	return hr;

}

//========================================
//
// 平面メッシュの生成
//
//========================================
HRESULT InstancingMeshRenderer::MakePlane(const std::string tag, int nNumBlockX, int nNumBlockZ, float fSizeBlockX, float fSizeBlockZ,
	float fTexSizeX, float fTexSizeZ)
{
	// メッシュデータの作成
	if (!CreateMeshData(tag)) return S_OK;


	// プリミティブ種別設定
	m_mesh->primitiveType = PT_TRIANGLESTRIP;
	// 頂点数の設定
	m_mesh->nNumVertex = (nNumBlockX + 1) * (nNumBlockZ + 1);
	// インデックス数の設定(縮退ポリゴン用を考慮する)
	m_mesh->nNumIndex = (nNumBlockX + 1) * 2 * nNumBlockZ + (nNumBlockZ - 1) * 2;
	// 頂点配列の作成
	VERTEX_3D* pVertexWk = new VERTEX_3D[m_mesh->nNumVertex];
	// インデックス配列の作成
	int* pIndexWk = new int[m_mesh->nNumIndex];
	// 頂点配列の中身を埋める
	VERTEX_3D* pVtx = pVertexWk;
	for (int z = 0; z < nNumBlockZ + 1; ++z) {
		for (int x = 0; x < nNumBlockX + 1; ++x) {

			// 頂点座標の設定
			pVtx->vtx.x = x * fSizeBlockX - (nNumBlockX * 0.5f) * fSizeBlockX;
			pVtx->vtx.y = 0.0f;
			pVtx->vtx.z = -z * fSizeBlockZ + (nNumBlockZ * 0.5f) * fSizeBlockZ;
			// 法線の設定
			pVtx->nor = XMFLOAT3(0.0f, 1.0f, 0.0f);
			// 反射光の設定
			pVtx->diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			// テクスチャ座標の設定
			pVtx->tex.x = fTexSizeX * x;
			pVtx->tex.y = fTexSizeZ * z;
			++pVtx;
		}
	}
	//インデックス配列の中身を埋める
	int* pIdx = pIndexWk;
	for (int z = 0; z < nNumBlockZ; ++z) {
		if (z > 0) {
			// 縮退ポリゴンのためのダブりの設定
			*pIdx++ = (z + 1) * (nNumBlockX + 1);
		}
		for (int x = 0; x < nNumBlockX + 1; ++x) {
			*pIdx++ = (z + 1) * (nNumBlockX + 1) + x;
			*pIdx++ = z * (nNumBlockX + 1) + x;
		}
		if (z < nNumBlockZ - 1) {
			// 縮退ポリゴンのためのダブりの設定
			*pIdx++ = z * (nNumBlockX + 1) + nNumBlockX;
		}
	}

	ID3D11Device* pDevice = GetDevice();
	// 頂点バッファ/インデックス バッファ生成
	HRESULT hr = MakeInstancingMeshVertex(pDevice, m_mesh, pVertexWk, pIndexWk);
	// 一時配列の解放
	delete[] pVertexWk;
	delete[] pIndexWk;
	return hr;
}

//========================================
//
// キューブメッシュの生成
//
//========================================
HRESULT InstancingMeshRenderer::MakeCube(const std::string tag)
{
	// メッシュデータの作成
	if (!CreateMeshData(tag)) return S_OK;


#define	SIZE_X			(0.5f)											// 立方体のサイズ(X方向)
#define	SIZE_Y			(0.5f)											// 立方体のサイズ(Y方向)
#define	SIZE_Z			(0.5f)											// 立方体のサイズ(Z方向)

#define CUBE_VERTEX		(24)
#define CUBE_INDEX		(36)

	// プリミティブ設定
	m_mesh->primitiveType = PT_TRIANGLE;

	VERTEX_3D	vertexWk[CUBE_VERTEX];	// 頂点情報格納ワーク
	int			indexWk[CUBE_INDEX];	// インデックス格納ワーク

	// 頂点数
	m_mesh->nNumVertex = CUBE_VERTEX;

	// 頂点座標の設定
	// 前
	vertexWk[0].vtx = XMFLOAT3(-SIZE_X, SIZE_Y, -SIZE_Z);
	vertexWk[1].vtx = XMFLOAT3(SIZE_X, SIZE_Y, -SIZE_Z);
	vertexWk[2].vtx = XMFLOAT3(-SIZE_X, -SIZE_Y, -SIZE_Z);
	vertexWk[3].vtx = XMFLOAT3(SIZE_X, -SIZE_Y, -SIZE_Z);
	// 右
	vertexWk[4].vtx = XMFLOAT3(SIZE_X, SIZE_Y, -SIZE_Z);
	vertexWk[5].vtx = XMFLOAT3(SIZE_X, SIZE_Y, SIZE_Z);
	vertexWk[6].vtx = XMFLOAT3(SIZE_X, -SIZE_Y, -SIZE_Z);
	vertexWk[7].vtx = XMFLOAT3(SIZE_X, -SIZE_Y, SIZE_Z);
	// 上
	vertexWk[8].vtx = XMFLOAT3(-SIZE_X, SIZE_Y, SIZE_Z);
	vertexWk[9].vtx = XMFLOAT3(SIZE_X, SIZE_Y, SIZE_Z);
	vertexWk[10].vtx = XMFLOAT3(-SIZE_X, SIZE_Y, -SIZE_Z);
	vertexWk[11].vtx = XMFLOAT3(SIZE_X, SIZE_Y, -SIZE_Z);
	// 後
	vertexWk[12].vtx = XMFLOAT3(SIZE_X, SIZE_Y, SIZE_Z);
	vertexWk[13].vtx = XMFLOAT3(-SIZE_X, SIZE_Y, SIZE_Z);
	vertexWk[14].vtx = XMFLOAT3(SIZE_X, -SIZE_Y, SIZE_Z);
	vertexWk[15].vtx = XMFLOAT3(-SIZE_X, -SIZE_Y, SIZE_Z);
	// 左
	vertexWk[16].vtx = XMFLOAT3(-SIZE_X, SIZE_Y, SIZE_Z);
	vertexWk[17].vtx = XMFLOAT3(-SIZE_X, SIZE_Y, -SIZE_Z);
	vertexWk[18].vtx = XMFLOAT3(-SIZE_X, -SIZE_Y, SIZE_Z);
	vertexWk[19].vtx = XMFLOAT3(-SIZE_X, -SIZE_Y, -SIZE_Z);
	// 下
	vertexWk[20].vtx = XMFLOAT3(SIZE_X, -SIZE_Y, SIZE_Z);
	vertexWk[21].vtx = XMFLOAT3(-SIZE_X, -SIZE_Y, SIZE_Z);
	vertexWk[22].vtx = XMFLOAT3(SIZE_X, -SIZE_Y, -SIZE_Z);
	vertexWk[23].vtx = XMFLOAT3(-SIZE_X, -SIZE_Y, -SIZE_Z);


	// 法線ベクトルの設定
	// 前
	vertexWk[0].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertexWk[1].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertexWk[2].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertexWk[3].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	// 右
	vertexWk[4].nor = XMFLOAT3(1.0f, 0.0f, 0.0f);
	vertexWk[5].nor = XMFLOAT3(1.0f, 0.0f, 0.0f);
	vertexWk[6].nor = XMFLOAT3(1.0f, 0.0f, 0.0f);
	vertexWk[7].nor = XMFLOAT3(1.0f, 0.0f, 0.0f);
	// 上
	vertexWk[8].nor = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertexWk[9].nor = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertexWk[10].nor = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertexWk[11].nor = XMFLOAT3(0.0f, 1.0f, 0.0f);
	// 後
	vertexWk[12].nor = XMFLOAT3(0.0f, 0.0f, 1.0f);
	vertexWk[13].nor = XMFLOAT3(0.0f, 0.0f, 1.0f);
	vertexWk[14].nor = XMFLOAT3(0.0f, 0.0f, 1.0f);
	vertexWk[15].nor = XMFLOAT3(0.0f, 0.0f, 1.0f);
	// 左
	vertexWk[16].nor = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	vertexWk[17].nor = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	vertexWk[18].nor = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	vertexWk[19].nor = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	// 下
	vertexWk[20].nor = XMFLOAT3(0.0f, -1.0f, 0.0f);
	vertexWk[21].nor = XMFLOAT3(0.0f, -1.0f, 0.0f);
	vertexWk[22].nor = XMFLOAT3(0.0f, -1.0f, 0.0f);
	vertexWk[23].nor = XMFLOAT3(0.0f, -1.0f, 0.0f);


	// 拡散反射光の設定
	for (int i = 0; i < CUBE_VERTEX; i++)
	{
		vertexWk[i].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// テクスチャ座標の設定
	for (int i = 0; i < CUBE_VERTEX; i += 4)
	{
		vertexWk[0 + i].tex = XMFLOAT2(0.0f, 0.0f);
		vertexWk[1 + i].tex = XMFLOAT2(1.0f, 0.0f);
		vertexWk[2 + i].tex = XMFLOAT2(0.0f, 1.0f);
		vertexWk[3 + i].tex = XMFLOAT2(1.0f, 1.0f);
	}

	// インデックス数
	m_mesh->nNumIndex = CUBE_INDEX;

	// インデックス配列の設定
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
// スフィアメッシュの生成
//
//========================================
HRESULT InstancingMeshRenderer::MakeSphere(std::string tag, int nSegment, float fTexSplit, XMFLOAT3 pos)
{
	// メッシュデータの作成
	if (!CreateMeshData(tag)) return S_OK;


	// プリミティブ種別設定
	m_mesh->primitiveType = PT_TRIANGLE;
	float fScale = 0.5f;

	//頂点バッファ作成
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

	// インデックスバッファの作成
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

	// 頂点バッファ/インデックス バッファ生成
	ID3D11Device* pDevice = GetDevice();
	HRESULT hr = MakeInstancingMeshVertex(pDevice, m_mesh, pVertexWk, pIndexWk);
	// 一時配列の解放
	delete[] pVertexWk;
	delete[] pIndexWk;
	return hr;

}


HRESULT InstancingMeshRenderer::MakeSphere2(std::string tag,
	int nNumBlockX, int nNumBlockY, float fSize,
	float fTexSizeX, float fTexSizeY,
	float fPosX, float fPosY, float fPosZ)
{
	// メッシュデータの作成
	if (!CreateMeshData(tag)) return S_OK;


	// プリミティブ種別設定
	m_mesh->primitiveType = PT_TRIANGLESTRIP;
	// 頂点数の設定
	m_mesh->nNumVertex = (nNumBlockX + 1) * (nNumBlockY + 1);
	// インデックス数の設定(縮退ポリゴン用を考慮する)
	m_mesh->nNumIndex = (nNumBlockX + 1) * 2 * nNumBlockY + (nNumBlockY - 1) * 2;
	// 頂点配列の作成
	VERTEX_3D* pVertexWk = new VERTEX_3D[m_mesh->nNumVertex];
	// インデックス配列の作成
	int* pIndexWk = new int[m_mesh->nNumIndex];
	// 頂点配列の中身を埋める
	VERTEX_3D* pVtx = pVertexWk;

	for (int y = 0; y < nNumBlockY + 1; ++y) {
		for (int x = 0; x < nNumBlockX + 1; ++x) {

			// 頂点座標の設定
			pVtx->vtx.x = 0.0f;
			pVtx->vtx.y = 1.0f;
			pVtx->vtx.z = 0.0f;
			// 角度に対する回転マトリックスを求める
			XMMATRIX mR = XMMatrixRotationX(XMConvertToRadians(-x * (-180.0f / nNumBlockX)));
			mR *= XMMatrixRotationY(XMConvertToRadians(-y * (360.0f / nNumBlockY)));
			// 座標を回転マトリックスで回転させる
			XMVECTOR v = XMLoadFloat3(&pVtx->vtx);
			v = XMVector3TransformCoord(v, mR);
			v = XMVector3Normalize(v);
			XMStoreFloat3(&pVtx->vtx, v);

			// 法線の設定
			pVtx->nor = pVtx->vtx;

			// 大きさ
			pVtx->vtx.x *= fSize;
			pVtx->vtx.y *= fSize;
			pVtx->vtx.z *= fSize;

			// 位置
			pVtx->vtx.x += fPosX;
			pVtx->vtx.y += fPosY;
			pVtx->vtx.z += fPosZ;

			// 反射光の設定
			pVtx->diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			// テクスチャ座標の設定
			pVtx->tex.x = fTexSizeX * x;
			pVtx->tex.y = fTexSizeY * y;
			++pVtx;
		}
	}
	//インデックス配列の中身を埋める
	int* pIdx = pIndexWk;
	for (int z = 0; z < nNumBlockY; ++z) {
		if (z > 0) {
			// 縮退ポリゴンのためのダブりの設定
			*pIdx++ = (z + 1) * (nNumBlockX + 1);
		}
		for (int x = 0; x < nNumBlockX + 1; ++x) {
			*pIdx++ = (z + 1) * (nNumBlockX + 1) + x;
			*pIdx++ = z * (nNumBlockX + 1) + x;
		}
		if (z < nNumBlockY - 1) {
			// 縮退ポリゴンのためのダブりの設定
			*pIdx++ = z * (nNumBlockX + 1) + nNumBlockX;
		}
	}

	ID3D11Device* pDevice = GetDevice();
	// 頂点バッファ/インデックス バッファ生成
	HRESULT hr = MakeInstancingMeshVertex(pDevice, m_mesh, pVertexWk, pIndexWk);
	// 一時配列の解放
	delete[] pVertexWk;
	delete[] pIndexWk;
	return hr;
}

//========================================
//
// 四面体 Tetraheron
//
//========================================
HRESULT InstancingMeshRenderer::MakeTetraheron(const std::string tag)
{
	// メッシュデータの作成
	if (!CreateMeshData(tag)) return S_OK;

	// プリミティブ設定
	m_mesh->primitiveType = PT_TRIANGLE;

	// 頂点数
	const int nVertexNum = 3 * 4;
	const int nIndexNum = 3 * 4;

	VERTEX_3D	vertexWk[nVertexNum];	// 頂点情報格納ワーク
	int			indexWk[nIndexNum];	// インデックス格納ワーク

	// 使いまわす頂点座標
	XMFLOAT3 ver[4] = {
		XMFLOAT3(-0.81649658, - 0.47140452,   0.33333333),
		XMFLOAT3( 0.81649658, - 0.47140452,   0.33333333),
		XMFLOAT3( 0.00000000,   0.00000000, - 1.00000000),
		XMFLOAT3( 0.00000000,   0.94280904,   0.33333333),
	};

	// 使いまわすインデックス
	int index[nIndexNum] = {
		1, 2, 4,
		1, 4, 3,
		1, 3, 2,
		2, 3, 4,
	};

	// 頂点数
	m_mesh->nNumVertex = nVertexNum;

	// 頂点座標の設定
	for (int i = 0; i < nVertexNum; ++i)
	{
		Vector3 halfV = ver[index[i] - 1];
		halfV /= 2;
		vertexWk[i].vtx = *halfV.GetFloat3();
	}

	// 法線ベクトルの設定
	for (int i = 0; i < nVertexNum; i += 3)
	{
		XMFLOAT3 n = *Vector3::Cross(ver[index[i + 0] - 1], ver[index[i + 1] - 1], ver[index[i + 2] - 1]).GetFloat3();
		vertexWk[i + 0].nor = n;
		vertexWk[i + 1].nor = n;
		vertexWk[i + 2].nor = n;
	}

	// 拡散反射光の設定
	for (int i = 0; i < nVertexNum; i++)
	{
		vertexWk[i].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// テクスチャ座標の設定
	for (int i = 0; i < nVertexNum; i += 4)
	{
		vertexWk[0 + i].tex = XMFLOAT2(0.0f, 0.0f);
		vertexWk[1 + i].tex = XMFLOAT2(1.0f, 0.0f);
		vertexWk[2 + i].tex = XMFLOAT2(0.0f, 1.0f);
		vertexWk[3 + i].tex = XMFLOAT2(1.0f, 1.0f);
	}

	// インデックス数
	m_mesh->nNumIndex = nIndexNum;

	// インデックス配列の設定
	for (int i = 0; i < nIndexNum; ++i)
	{
		indexWk[i] = i;
	}

	ID3D11Device* pDevice = GetDevice();
	return MakeInstancingMeshVertex(pDevice, m_mesh, vertexWk, indexWk);
}

//========================================
//
// 八面体 Octahedron
//
//========================================
HRESULT InstancingMeshRenderer::MakeOctahedron(const std::string tag)
{
	// メッシュデータの作成
	if (!CreateMeshData(tag)) return S_OK;

	// プリミティブ設定
	m_mesh->primitiveType = PT_TRIANGLE;

	// 頂点数
	const int nVertexNum = 3 * 8;
	const int nIndexNum  = 3 * 8;

	VERTEX_3D	vertexWk[nVertexNum];	// 頂点情報格納ワーク
	int			indexWk[nIndexNum];	// インデックス格納ワーク

	// 使いまわす頂点座標
	XMFLOAT3 ver[6] = {
		XMFLOAT3(-0.70710678, - 0.70710678,  0.00000000),
		XMFLOAT3(-0.70710678,  0.70710678,  0.00000000),
		XMFLOAT3(0.70710678,  0.70710678,  0.00000000),
		XMFLOAT3(0.70710678, - 0.70710678,  0.00000000),
		XMFLOAT3(0.00000000,  0.00000000, - 1.00000000),
		XMFLOAT3(0.00000000,  0.00000000,  1.00000000),
	};

	// 使いまわすインデックス
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

	// 頂点数
	m_mesh->nNumVertex = nVertexNum;

	// 頂点座標の設定
	for (int i = 0; i < nVertexNum; ++i)
	{
		Vector3 halfV = ver[index[i] - 1];
		halfV /= 2;
		vertexWk[i].vtx = *halfV.GetFloat3();
	}

	// 法線ベクトルの設定
	for (int i = 0; i < nVertexNum; i += 3)
	{
		XMFLOAT3 n = *Vector3::Cross(ver[index[i + 0] - 1], ver[index[i + 1] - 1], ver[index[i + 2] - 1]).GetFloat3();
		vertexWk[i + 0].nor = n;
		vertexWk[i + 1].nor = n;
		vertexWk[i + 2].nor = n;
	}

	// 拡散反射光の設定
	for (int i = 0; i < nVertexNum; i++)
	{
		vertexWk[i].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// テクスチャ座標の設定
	for (int i = 0; i < nVertexNum; i += 4)
	{
		vertexWk[0 + i].tex = XMFLOAT2(0.0f, 0.0f);
		vertexWk[1 + i].tex = XMFLOAT2(1.0f, 0.0f);
		vertexWk[2 + i].tex = XMFLOAT2(0.0f, 1.0f);
		vertexWk[3 + i].tex = XMFLOAT2(1.0f, 1.0f);
	}

	// インデックス数
	m_mesh->nNumIndex = nIndexNum;

	// インデックス配列の設定
	for (int i = 0; i < nIndexNum; ++i)
	{
		indexWk[i] = i;
	}

	ID3D11Device* pDevice = GetDevice();
	return MakeInstancingMeshVertex(pDevice, m_mesh, vertexWk, indexWk);
}

//========================================
//
// 十二面体
//
//========================================
HRESULT InstancingMeshRenderer::MakeDodecahedron(const std::string tag)
{
	// メッシュデータの作成
	if (!CreateMeshData(tag)) return S_OK;

	// プリミティブ設定
	m_mesh->primitiveType = PT_TRIANGLE;

	// 頂点数
	const int nVertexNum = 3 * 36;
	const int nIndexNum = 3 * 36;

	VERTEX_3D	vertexWk[nVertexNum];	// 頂点情報格納ワーク
	int			indexWk[nIndexNum];	// インデックス格納ワーク

	// 使いまわす頂点座標
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

	// 使いまわすインデックス
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

	// 頂点数
	m_mesh->nNumVertex = nVertexNum;

	// 頂点座標の設定
	for (int i = 0; i < nVertexNum; ++i)
	{
		Vector3 halfV = ver[index[i] - 1];
		halfV /= 2;
		vertexWk[i].vtx = *halfV.GetFloat3();
	}

	// 法線ベクトルの設定
	for (int i = 0; i < nVertexNum; i += 3)
	{
		XMFLOAT3 n = *Vector3::Cross(ver[index[i + 0] - 1], ver[index[i + 1] - 1], ver[index[i + 2] - 1]).GetFloat3();
		vertexWk[i + 0].nor = n;
		vertexWk[i + 1].nor = n;
		vertexWk[i + 2].nor = n;
	}

	// 拡散反射光の設定
	for (int i = 0; i < nVertexNum; i++)
	{
		vertexWk[i].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// テクスチャ座標の設定
	for (int i = 0; i < nVertexNum; i += 4)
	{
		vertexWk[0 + i].tex = XMFLOAT2(0.0f, 0.0f);
		vertexWk[1 + i].tex = XMFLOAT2(1.0f, 0.0f);
		vertexWk[2 + i].tex = XMFLOAT2(0.0f, 1.0f);
		vertexWk[3 + i].tex = XMFLOAT2(1.0f, 1.0f);
	}

	// インデックス数
	m_mesh->nNumIndex = nIndexNum;

	// インデックス配列の設定
	for (int i = 0; i < nIndexNum; ++i)
	{
		indexWk[i] = i;
	}

	ID3D11Device* pDevice = GetDevice();
	return MakeInstancingMeshVertex(pDevice, m_mesh, vertexWk, indexWk);
}

//========================================
//
// 二十面体
//
//========================================
HRESULT InstancingMeshRenderer::MakeIcosahedron(const std::string tag)
{
	// メッシュデータの作成
	if (!CreateMeshData(tag)) return S_OK;

	// プリミティブ設定
	m_mesh->primitiveType = PT_TRIANGLE;

	// 頂点数
	const int nVertexNum = 3 * 20;
	const int nIndexNum = 3 * 20;

	VERTEX_3D	vertexWk[nVertexNum];	// 頂点情報格納ワーク
	int			indexWk[nIndexNum];	// インデックス格納ワーク

	// 使いまわす頂点座標
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

	// 使いまわすインデックス
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

	// 頂点数
	m_mesh->nNumVertex = nVertexNum;

	// 頂点座標の設定
	for (int i = 0; i < nVertexNum; ++i)
	{
		Vector3 halfV = ver[index[i] - 1];
		halfV /= 2;
		vertexWk[i].vtx = *halfV.GetFloat3();
	}

	// 法線ベクトルの設定
	for (int i = 0; i < nVertexNum; i += 3)
	{
		XMFLOAT3 n = *Vector3::Cross(ver[index[i + 0] - 1], ver[index[i + 1] - 1], ver[index[i + 2] - 1]).GetFloat3();
		vertexWk[i + 0].nor = n;
		vertexWk[i + 1].nor = n;
		vertexWk[i + 2].nor = n;
	}

	// 拡散反射光の設定
	for (int i = 0; i < nVertexNum; i++)
	{
		vertexWk[i].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// テクスチャ座標の設定
	for (int i = 0; i < nVertexNum; i += 4)
	{
		vertexWk[0 + i].tex = XMFLOAT2(0.0f, 0.0f);
		vertexWk[1 + i].tex = XMFLOAT2(1.0f, 0.0f);
		vertexWk[2 + i].tex = XMFLOAT2(0.0f, 1.0f);
		vertexWk[3 + i].tex = XMFLOAT2(1.0f, 1.0f);
	}

	// インデックス数
	m_mesh->nNumIndex = nIndexNum;

	// インデックス配列の設定
	for (int i = 0; i < nIndexNum; ++i)
	{
		indexWk[i] = i;
	}

	ID3D11Device* pDevice = GetDevice();
	return MakeInstancingMeshVertex(pDevice, m_mesh, vertexWk, indexWk);
}

//========================================
//
// S08
//
//========================================
HRESULT InstancingMeshRenderer::MakeS08(const std::string tag)
{
	// メッシュデータの作成
	if (!CreateMeshData(tag)) return S_OK;

	// プリミティブ設定
	m_mesh->primitiveType = PT_TRIANGLE;

	// 頂点数
	const int nVertexNum = 3 * 44;
	const int nIndexNum = 3 * 44;

	VERTEX_3D	vertexWk[nVertexNum];	// 頂点情報格納ワーク
	int			indexWk[nIndexNum];	// インデックス格納ワーク

	// 使いまわす頂点座標
	XMFLOAT3 ver[24] = {
		XMFLOAT3(-0.357407, - 0.862856,   0.357407),
		XMFLOAT3(-0.862856, - 0.357407,   0.357407),
		XMFLOAT3(-0.862856, - 0.357407, - 0.357407),
		XMFLOAT3(-0.357407, - 0.862856, - 0.357407),
		XMFLOAT3( 0.357407, - 0.862856, - 0.357407),
		XMFLOAT3( 0.357407, - 0.862856,   0.357407),
		XMFLOAT3( 0.357407, - 0.357407,   0.862856),
		XMFLOAT3(-0.357407, - 0.357407,   0.862856),
		XMFLOAT3(-0.862856,   0.357407,   0.357407),
		XMFLOAT3(-0.862856,   0.357407, - 0.357407),
		XMFLOAT3(-0.357407,   0.357407,   0.862856),
		XMFLOAT3(-0.357407,   0.862856,   0.357407),
		XMFLOAT3(-0.357407,   0.862856, - 0.357407),
		XMFLOAT3( 0.357407,   0.862856,   0.357407),
		XMFLOAT3( 0.357407,   0.862856, - 0.357407),
		XMFLOAT3( 0.357407,   0.357407,   0.862856),
		XMFLOAT3( 0.862856,   0.357407,   0.357407),
		XMFLOAT3( 0.862856,   0.357407, - 0.357407),
		XMFLOAT3( 0.862856, - 0.357407,   0.357407),
		XMFLOAT3( 0.862856, - 0.357407, - 0.357407),
		XMFLOAT3(-0.357407, - 0.357407, - 0.862856),
		XMFLOAT3( 0.357407, - 0.357407, - 0.862856),
		XMFLOAT3(-0.357407,   0.357407, - 0.862856),
		XMFLOAT3( 0.357407,   0.357407, - 0.862856),
	};

	// 使いまわすインデックス
	int index[nIndexNum] = {
		2, 4, 1,
		4, 6, 1,
		6, 8, 1,
		1, 8, 2,
		9, 3, 2,
		8, 9, 2,
		12, 10, 9,
		9, 11, 12,
		14, 13, 12,
		11, 14, 12,
		17, 15, 14,
		14, 16, 17,
		19, 18, 17,
		16, 19, 17,
		6, 20, 19,
		19, 7, 6,
		4, 3, 21,
		21, 5, 4,
		10, 21, 3,
		10, 13, 23,
		15, 23, 13,
		15, 18, 24,
		20, 24, 18,
		20, 5, 22,
		7, 11, 8,
		23, 22, 21,
		2, 3, 4,
		4, 5, 6,
		6, 7, 8,
		9, 10, 3,
		8, 11, 9,
		12, 13, 10,
		14, 15, 13,
		11, 16, 14,
		17, 18, 15,
		19, 20, 18,
		16, 7, 19,
		6, 5, 20,
		21, 22, 5,
		10, 23, 21,
		15, 24, 23,
		20, 22, 24,
		7, 16, 11,
		23, 24, 22,
	};

	// 頂点数
	m_mesh->nNumVertex = nVertexNum;

	// 頂点座標の設定
	for (int i = 0; i < nVertexNum; ++i)
	{
		Vector3 halfV = ver[index[i] - 1];
		halfV /= 2;
		vertexWk[i].vtx = *halfV.GetFloat3();
	}

	// 法線ベクトルの設定
	for (int i = 0; i < nVertexNum; i += 3)
	{
		XMFLOAT3 n = *Vector3::Cross(ver[index[i + 0] - 1], ver[index[i + 1] - 1], ver[index[i + 2] - 1]).GetFloat3();
		vertexWk[i + 0].nor = n;
		vertexWk[i + 1].nor = n;
		vertexWk[i + 2].nor = n;
	}

	// 拡散反射光の設定
	for (int i = 0; i < nVertexNum; i++)
	{
		vertexWk[i].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// テクスチャ座標の設定
	for (int i = 0; i < nVertexNum; i += 4)
	{
		vertexWk[0 + i].tex = XMFLOAT2(0.0f, 0.0f);
		vertexWk[1 + i].tex = XMFLOAT2(1.0f, 0.0f);
		vertexWk[2 + i].tex = XMFLOAT2(0.0f, 1.0f);
		vertexWk[3 + i].tex = XMFLOAT2(1.0f, 1.0f);
	}

	// インデックス数
	m_mesh->nNumIndex = nIndexNum;

	// インデックス配列の設定
	for (int i = 0; i < nIndexNum; ++i)
	{
		indexWk[i] = i;
	}

	ID3D11Device* pDevice = GetDevice();
	return MakeInstancingMeshVertex(pDevice, m_mesh, vertexWk, indexWk);
}

//========================================
//
// N15
//
//========================================
HRESULT InstancingMeshRenderer::MakeN15(const std::string tag)
{
	// メッシュデータの作成
	if (!CreateMeshData(tag)) return S_OK;

	// プリミティブ設定
	m_mesh->primitiveType = PT_TRIANGLE;

	// 頂点数
	const int nVertexNum = 3 * 16;
	const int nIndexNum = 3 * 16;

	VERTEX_3D	vertexWk[nVertexNum];	// 頂点情報格納ワーク
	int			indexWk[nIndexNum];	// インデックス格納ワーク

	// 使いまわす頂点座標
	XMFLOAT3 ver[10] = {
		XMFLOAT3(-0.414214, - 0.414214, 0.414214),
		XMFLOAT3(-0.414214, - 0.414214, - 0.414214),
		XMFLOAT3( 0.414214, - 0.414214, - 0.414214),
		XMFLOAT3( 0.414214, - 0.414214, 0.414214),
		XMFLOAT3( 0.414214,   0.414214, 0.414214),
		XMFLOAT3(-0.414214,   0.414214, 0.414214),
		XMFLOAT3(-1.000000,   0.000000, 0.000000),
		XMFLOAT3( 0.414214,   0.414214, - 0.414214),
		XMFLOAT3(-0.414214,   0.414214, - 0.414214),
		XMFLOAT3( 1.000000,   0.000000, 0.000000),
	};

	// 使いまわすインデックス
	int index[nIndexNum] = {
		2, 4, 1,
		4, 6, 1,
		1, 6, 7,
		1, 7, 2,
		5, 9, 6,
		6, 9, 7,
		5, 4, 10,
		5, 10, 8,
		4, 3, 10,
		9, 3, 2,
		2, 7, 9,
		8, 10, 3,
		2, 3, 4,
		4, 5, 6,
		5, 8, 9,
		9, 8, 3,
	};

	// 頂点数
	m_mesh->nNumVertex = nVertexNum;

	// 頂点座標の設定
	for (int i = 0; i < nVertexNum; ++i)
	{
		Vector3 halfV = ver[index[i] - 1];
		halfV /= 2;
		vertexWk[i].vtx = *halfV.GetFloat3();
	}

	// 法線ベクトルの設定
	for (int i = 0; i < nVertexNum; i += 3)
	{
		XMFLOAT3 n = *Vector3::Cross(ver[index[i + 0] - 1], ver[index[i + 1] - 1], ver[index[i + 2] - 1]).GetFloat3();
		vertexWk[i + 0].nor = n;
		vertexWk[i + 1].nor = n;
		vertexWk[i + 2].nor = n;
	}

	// 拡散反射光の設定
	for (int i = 0; i < nVertexNum; i++)
	{
		vertexWk[i].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// テクスチャ座標の設定
	for (int i = 0; i < nVertexNum; i += 4)
	{
		vertexWk[0 + i].tex = XMFLOAT2(0.0f, 0.0f);
		vertexWk[1 + i].tex = XMFLOAT2(1.0f, 0.0f);
		vertexWk[2 + i].tex = XMFLOAT2(0.0f, 1.0f);
		vertexWk[3 + i].tex = XMFLOAT2(1.0f, 1.0f);
	}

	// インデックス数
	m_mesh->nNumIndex = nIndexNum;

	// インデックス配列の設定
	for (int i = 0; i < nIndexNum; ++i)
	{
		indexWk[i] = i;
	}

	ID3D11Device* pDevice = GetDevice();
	return MakeInstancingMeshVertex(pDevice, m_mesh, vertexWk, indexWk);
}

//========================================
//
// S06
//
//========================================
HRESULT InstancingMeshRenderer::MakeS06(const std::string tag)
{
	// メッシュデータの作成
	if (!CreateMeshData(tag)) return S_OK;

	// プリミティブ設定
	m_mesh->primitiveType = PT_TRIANGLE;

	// 頂点数
	const int nVertexNum = 3 * 116;
	const int nIndexNum = 3 * 116;

	VERTEX_3D*	vertexWk = new VERTEX_3D[nVertexNum];	// 頂点情報格納ワーク
	int			indexWk[nIndexNum];	// インデックス格納ワーク

	// 使いまわす頂点座標
	XMFLOAT3 ver[60] = {
		XMFLOAT3(-0.201774, - 0.277718, 0.939234),
		XMFLOAT3( 0.201774, - 0.277718, 0.939234),
		XMFLOAT3( 0.326477,   0.106079, 0.939234),
		XMFLOAT3( 0.000000,   0.343278, 0.939234),
		XMFLOAT3(-0.326477,   0.106079, 0.939234),
		XMFLOAT3(-0.652955,   0.212158, 0.727076),
		XMFLOAT3(-0.854729, - 0.065560, 0.514918),
		XMFLOAT3(-0.730026, - 0.449358, 0.514918),
		XMFLOAT3(-0.403548, - 0.555437, 0.727076),
		XMFLOAT3(-0.201774, - 0.833155, 0.514918),
		XMFLOAT3( 0.201774, - 0.833155, 0.514918),
		XMFLOAT3( 0.403548, - 0.555437, 0.727076),
		XMFLOAT3(-0.730026, - 0.661515, 0.171639),
		XMFLOAT3(-0.403548, - 0.898715, 0.171639),
		XMFLOAT3(-0.201774, - 0.964275, - 0.171639),
		XMFLOAT3( 0.201774, - 0.964275, - 0.171639),
		XMFLOAT3( 0.403548, - 0.898715, 0.171639),
		XMFLOAT3( 0.730026, - 0.661515, 0.171639),
		XMFLOAT3( 0.730026, - 0.449358, 0.514918),
		XMFLOAT3( 0.854729, - 0.065560, 0.514918),
		XMFLOAT3( 0.652955,   0.212158, 0.727076),
		XMFLOAT3( 0.000000,   0.686557, 0.727076),
		XMFLOAT3(-0.326477,   0.792636, 0.514918),
		XMFLOAT3(-0.652955,   0.555436, 0.514918),
		XMFLOAT3(-0.854729,   0.489876, 0.171639),
		XMFLOAT3(-0.979432,   0.106079, 0.171639),
		XMFLOAT3(-0.979432, - 0.106079, - 0.171639),
		XMFLOAT3(-0.854729, - 0.489876, - 0.171639),
		XMFLOAT3(-0.652955, - 0.555436, - 0.514918),
		XMFLOAT3(-0.326477, - 0.792636, - 0.514918),
		XMFLOAT3( 0.000000, - 0.686557, - 0.727076),
		XMFLOAT3( 0.326477, - 0.792636, - 0.514918),
		XMFLOAT3( 0.652955, - 0.555436, - 0.514918),
		XMFLOAT3( 0.854729, - 0.489876, - 0.171639),
		XMFLOAT3( 0.979432, - 0.106079, - 0.171639),
		XMFLOAT3( 0.979432,   0.106079, 0.171639),
		XMFLOAT3( 0.854729,   0.489876, 0.171639),
		XMFLOAT3( 0.652955,   0.555436, 0.514918),
		XMFLOAT3( 0.326477,   0.792636, 0.514918),
		XMFLOAT3(-0.201774,   0.964275, 0.171639),
		XMFLOAT3(-0.403548,   0.898715, - 0.171639),
		XMFLOAT3(-0.730026,   0.661515, - 0.171639),
		XMFLOAT3(-0.730026,   0.449358, - 0.514918),
		XMFLOAT3(-0.854729,   0.065560, - 0.514918),
		XMFLOAT3(-0.652955, - 0.212158, - 0.727076),
		XMFLOAT3(-0.326477, - 0.106079, - 0.939234),
		XMFLOAT3( 0.000000, - 0.343278, - 0.939234),
		XMFLOAT3( 0.326477, - 0.106079, - 0.939234),
		XMFLOAT3( 0.652955, - 0.212158, - 0.727076),
		XMFLOAT3( 0.854729,   0.065560, - 0.514918),
		XMFLOAT3( 0.730026,   0.449358, - 0.514918),
		XMFLOAT3( 0.730026,   0.661515, - 0.171639),
		XMFLOAT3( 0.403548,   0.898715, - 0.171639),
		XMFLOAT3( 0.201774,   0.964275, 0.171639),
		XMFLOAT3(-0.201774,   0.833155, - 0.514918),
		XMFLOAT3(-0.403548,   0.555437, - 0.727076),
		XMFLOAT3(-0.201774,   0.277718, - 0.939234),
		XMFLOAT3( 0.201774,   0.277718, - 0.939234),
		XMFLOAT3( 0.403548,   0.555437, - 0.727076),
		XMFLOAT3( 0.201774,   0.833155, - 0.514918),
	};

	// 使いまわすインデックス
	int index[nIndexNum] = {
		3, 4, 5,
		5, 7, 9,
		9, 11, 2,
		13, 14, 10,
		14, 16, 11,
		17, 19, 12,
		19, 21, 2,
		23, 24, 6,
		24, 25, 26,
		26, 28, 8,
		28, 30, 14,
		31, 32, 16,
		32, 34, 17,
		34, 36, 19,
		37, 38, 21,
		38, 22, 3,
		23, 41, 25,
		42, 44, 26,
		45, 29, 28,
		45, 47, 30,
		47, 49, 32,
		49, 50, 35,
		50, 52, 36,
		52, 54, 38,
		54, 40, 23,
		41, 56, 43,
		56, 46, 44,
		57, 58, 48,
		51, 50, 49,
		59, 53, 52,
		60, 41, 54,
		60, 58, 56,
		5, 1, 2,
		2, 3, 5,
		9, 1, 5,
		5, 6, 7,
		7, 8, 9,
		2, 1, 9,
		9, 10, 11,
		11, 12, 2,
		10, 9, 8,
		8, 13, 10,
		11, 10, 14,
		14, 15, 16,
		16, 17, 11,
		12, 11, 17,
		17, 18, 19,
		2, 12, 19,
		19, 20, 21,
		21, 3, 2,
		6, 5, 23,
		5, 4, 23,
		4, 22, 23,
		7, 6, 26,
		6, 24, 26,
		8, 7, 26,
		26, 27, 28,
		28, 13, 8,
		14, 13, 28,
		28, 29, 30,
		30, 15, 14,
		16, 15, 30,
		30, 31, 16,
		17, 16, 32,
		32, 33, 34,
		34, 18, 17,
		19, 18, 34,
		34, 35, 36,
		36, 20, 19,
		21, 20, 36,
		36, 37, 21,
		3, 21, 38,
		38, 39, 22,
		22, 4, 3,
		25, 24, 23,
		23, 40, 41,
		41, 42, 25,
		26, 25, 42,
		42, 43, 44,
		44, 27, 26,
		28, 27, 44,
		44, 45, 28,
		30, 29, 45,
		45, 46, 47,
		47, 31, 30,
		32, 31, 47,
		47, 48, 49,
		49, 33, 32,
		34, 33, 35,
		33, 49, 35,
		36, 35, 50,
		50, 51, 52,
		52, 37, 36,
		38, 37, 52,
		52, 53, 54,
		54, 39, 38,
		22, 39, 23,
		39, 54, 23,
		43, 42, 41,
		41, 55, 56,
		44, 43, 56,
		56, 57, 46,
		46, 45, 44,
		47, 46, 48,
		46, 57, 48,
		49, 48, 58,
		58, 59, 49,
		59, 51, 49,
		52, 51, 59,
		59, 60, 53,
		54, 53, 60,
		60, 55, 41,
		41, 40, 54,
		56, 55, 60,
		60, 59, 58,
		58, 57, 56,
	};

	// 頂点数
	m_mesh->nNumVertex = nVertexNum;

	// 頂点座標の設定
	for (int i = 0; i < nVertexNum; ++i)
	{
		Vector3 halfV = ver[index[i] - 1];
		halfV /= 2;
		vertexWk[i].vtx = *halfV.GetFloat3();
	}

	// 法線ベクトルの設定
	for (int i = 0; i < nVertexNum; i += 3)
	{
		XMFLOAT3 n = *Vector3::Cross(ver[index[i + 0] - 1], ver[index[i + 1] - 1], ver[index[i + 2] - 1]).GetFloat3();
		vertexWk[i + 0].nor = n;
		vertexWk[i + 1].nor = n;
		vertexWk[i + 2].nor = n;
	}

	// 拡散反射光の設定
	for (int i = 0; i < nVertexNum; i++)
	{
		vertexWk[i].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// テクスチャ座標の設定
	for (int i = 0; i < nVertexNum; i += 4)
	{
		vertexWk[0 + i].tex = XMFLOAT2(0.0f, 0.0f);
		vertexWk[1 + i].tex = XMFLOAT2(1.0f, 0.0f);
		vertexWk[2 + i].tex = XMFLOAT2(0.0f, 1.0f);
		vertexWk[3 + i].tex = XMFLOAT2(1.0f, 1.0f);
	}

	// インデックス数
	m_mesh->nNumIndex = nIndexNum;

	// インデックス配列の設定
	for (int i = 0; i < nIndexNum; ++i)
	{
		indexWk[i] = i;
	}

	ID3D11Device* pDevice = GetDevice();
	HRESULT hr = MakeInstancingMeshVertex(pDevice, m_mesh, vertexWk, indexWk);
	delete[] vertexWk;

	return hr;
}


//========================================
//
// ベースカラーテクスチャのセット
//
//========================================
void InstancingMeshRenderer::SetDiffuseTexture(const char* filename)
{
	// メッシュ取得
	auto pInstancingMesh = m_mesh;
	// テクスチャ
	if (pInstancingMesh)
		pInstancingMesh->pTexture = CTexture::GetTexture(GetDevice(), filename);
}

//========================================
//
// ベースカラーテクスチャのセット
//
//========================================
void InstancingMeshRenderer::SetDiffuseTexture(const wchar_t* filename)
{
	// メッシュ取得
	auto pInstancingMesh = m_mesh;
	// テクスチャ
	if (pInstancingMesh)
		pInstancingMesh->pTexture = CTexture::GetTexture(GetDevice(), filename);
}