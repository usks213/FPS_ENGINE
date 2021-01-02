//======================================================================
//											main.cpp
//	ECS設計フレームワーク(データ指向じゃない．．．)
//
//======================================================================
//	author :	AT12A 05 宇佐美晃之
//======================================================================
//	開発履歴
//
//	2020/12/21	ECSベースで１から開発スタート
//				Object,Entity,Component の作成から
//
//	2020/12/22	World,EntityManager,System の作成
//	
//	2020/12/23	Transform,TransformSystem の作成
//				システムとコンポーネントのやり取りうまくいった！！
//				Renderer, RendererSystem の作成
//
//	2020/12/26	Collider,CollisionSystem の作成
//				BoxCollider,SphereCollider の作成
//
//	2020/12/27	GameObject,Script,ScriptSystem の作成
//				ScriptBaseLayout の作成
//
//	2020/12/28	Rigidbody,RigidbodySystem の作成
//				物理計算実装(上手くいかない)
//
//	2020/12/30	当たり判定修正 物理計算上手くいった！！
//
//	2020/12/31	FPS視点作成、操作や弾生成など
//
//	2021/01/01	あけましておめでとうございます！！
//
//	2021/01/02	Animator2D,Animation2D,Animation2DSystem の作成
//				SpriteRenderer,BillboardRenderer,AssimpRenderer の作成
//
//
//======================================================================
#include "main.h"
#include "System/input.h"
#include "System/polygon.h"
#include "System/debugproc.h"
#include "System/Sound.h"

// ECS
#include "ECS/Object/ObjectManager.h"
#include "ECS/Entity/EntityManager.h"
#include "ECS/System/ISystem.h"
#include "ECS/Entity/IEntity.h"
#include "ECS/Component/IComponent.h"
#include "ECSEntity/GameObject.h"

// システム
#include "ECSSystem/TransformSystem.h"
#include "ECSSystem/RendererSystem.h"
#include "ECSSystem/CollisionSystem.h"
#include "ECSSystem/RigidbodySystem.h"
#include "ECSSystem/ScriptSystem.h"

// コンポーネント
#include "ECSCompoent/Transform.h"
#include "ECSCompoent/MeshRenderer.h"
#include "ECSCompoent/Rigidbody.h"
#include "ECSCompoent/SphereCollider.h"
#include "ECSCompoent/BoxCollider.h"

// レンダラー
#include "Renderer/Camera.h"
#include "Renderer/Light.h"
#include "Renderer/mesh.h"
#include "Renderer/AssimpModel.h"

// スクリプト
#include "../Scripts/PlayerScript.h"



//-------- ライブラリのリンク
#pragma comment(lib, "winmm")
#pragma comment(lib, "imm32")
#pragma comment(lib, "d3d11")

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define CLASS_NAME		_T("AppClass")					// ウインドウのクラス名
#define WINDOW_NAME		_T("モータくん")			// ウインドウのキャプション名

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int OnCreate(HWND hWnd, LPCREATESTRUCT lpcs);
HRESULT Init(HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);

//*****************************************************************************
// グローバル変数:
//*****************************************************************************
HWND						g_hWnd;					// メイン ウィンドウ ハンドル
HINSTANCE					g_hInst;				// インスタンス ハンドル

ID3D11Device*				g_pDevice;				// デバイス
ID3D11DeviceContext*		g_pDeviceContext;		// デバイス コンテキスト
IDXGISwapChain*				g_pSwapChain;			// スワップチェーン
ID3D11RenderTargetView*		g_pRenderTargetView;	// フレームバッファ
ID3D11Texture2D*			g_pDepthStencilTexture;	// Zバッファ用メモリ
ID3D11DepthStencilView*		g_pDepthStencilView;	// Zバッファ
UINT						g_uSyncInterval = 1;	// 垂直同期 (0:無, 1:有)
ID3D11RasterizerState*		g_pRs[MAX_CULLMODE];	// ラスタライザ ステート
ID3D11BlendState*			g_pBlendState[MAX_BLENDSTATE];// ブレンド ステート
ID3D11DepthStencilState*	g_pDSS[2];				// Z/ステンシル ステート

int							g_nCountFPS;			// FPSカウンタ

CCamera						g_camera;				// カメラ
CLight						g_light;				// 光源

using namespace ECS;

World g_world;


//=============================================================================
// メイン関数
//=============================================================================
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// 未使用宣言
	UNREFERENCED_PARAMETER(lpCmdLine);		// 未使用宣言

	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;

	WNDCLASSEX wcex = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAINFRM)),
		LoadCursor(nullptr, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		nullptr,
		CLASS_NAME,
		nullptr
	};
	MSG msg;
	
	// COM初期化
	if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED))) {
		MessageBox(NULL, _T("COMの初期化に失敗しました。"), _T("error"), MB_OK);
		return -1;
	}

	// インスタンス ハンドル保存
	g_hInst = hInstance;

	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// クライアント領域サイズからウィンドウ サイズ算出
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION
		| WS_SYSMENU | WS_BORDER | WS_MINIMIZEBOX;
	DWORD dwExStyle = 0;
	RECT rc = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	AdjustWindowRectEx(&rc, dwStyle, FALSE, dwExStyle);

	// ウィンドウの作成
	g_hWnd = CreateWindowEx(dwExStyle,
		CLASS_NAME,
		WINDOW_NAME,
		dwStyle,
		CW_USEDEFAULT,		// ウィンドウの左座標
		CW_USEDEFAULT,		// ウィンドウの上座標
		rc.right - rc.left,	// ウィンドウ横幅
		rc.bottom - rc.top,	// ウィンドウ縦幅
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	// フレームカウント初期化
	timeBeginPeriod(1);				// 分解能を設定
	dwExecLastTime = dwFPSLastTime = timeGetTime();
	dwCurrentTime = dwFrameCount = 0;

	// ウインドウの表示(初期化処理の後に呼ばないと駄目)
	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);
	
	// DirectXの初期化(ウィンドウを作成してから行う)
	if (FAILED(Init(g_hWnd, true))) {
		return -1;
	}

	// メッセージループ
	for (;;) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				// PostQuitMessage()が呼ばれたらループ終了
				break;
			} else {
				// メッセージの翻訳とディスパッチ
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		} else {
			dwCurrentTime = timeGetTime();
			if ((dwCurrentTime - dwFPSLastTime) >= 500) {	// 0.5秒ごとに実行
				g_nCountFPS = dwFrameCount * 1000 / (dwCurrentTime - dwFPSLastTime);
				dwFPSLastTime = dwCurrentTime;
				dwFrameCount = 0;
			}
			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60)) {
				dwExecLastTime = dwCurrentTime;
				// 更新処理
				Update();
			}
			// 描画処理
			Draw();
			dwFrameCount++;
		}
	}

	// タイマ設定を元に戻す
	timeEndPeriod(1);

	// 終了処理
	Uninit();

	// ウィンドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, g_hInst);

	// COM終了処理
	CoUninitialize();

	return (int)msg.wParam;
}

//=============================================================================
// プロシージャ
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:					//----- ウィンドウが生成された
		return OnCreate(hWnd, (LPCREATESTRUCT)lParam);
	case WM_DESTROY:				//----- ウィンドウ破棄指示がきた
		PostQuitMessage(0);				// システムにスレッドの終了を要求
		break;
	case WM_KEYDOWN:				//----- キーボードが押された
		switch (wParam) {
		case VK_ESCAPE:					// [ESC]キーが押された
			PostMessage(hWnd, WM_CLOSE, 0, 0);	// [x]が押されたように振舞う
			return 0;
		}
		break;
	case WM_MENUCHAR:
		return MNC_CLOSE << 16;			// [Alt]+[Enter]時のBEEPを抑止
	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//=============================================================================
// WM_CREATEメッセージハンドラ
//=============================================================================
int OnCreate(HWND hWnd, LPCREATESTRUCT lpcs)
{
	// クライアント領域サイズをSCREEN_WIDTH×SCREEN_HEIGHTに再設定.
	RECT rcClnt;
	GetClientRect(hWnd, &rcClnt);
	rcClnt.right -= rcClnt.left;
	rcClnt.bottom -= rcClnt.top;
	if (rcClnt.right != SCREEN_WIDTH || rcClnt.bottom != SCREEN_HEIGHT) {
		RECT rcWnd;
		GetWindowRect(hWnd, &rcWnd);
		SIZE sizeWnd;
		sizeWnd.cx = (rcWnd.right - rcWnd.left) - rcClnt.right + SCREEN_WIDTH;
		sizeWnd.cy = (rcWnd.bottom - rcWnd.top) - rcClnt.bottom + SCREEN_HEIGHT;
		SetWindowPos(hWnd, nullptr, 0, 0, sizeWnd.cx, sizeWnd.cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER);
	}

	// IME無効化
	ImmAssociateContext(hWnd, nullptr);

	return 0;	// -1を返すとCreateWindow[Ex]が失敗する.
}

//=============================================================================
// バックバッファ生成
//=============================================================================
HRESULT CreateBackBuffer(void)
{
	ID3D11Device* pDevice = g_pDevice;

	// レンダーターゲットビュー生成
	ID3D11Texture2D* pBackBuffer = nullptr;
	g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	g_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
	SAFE_RELEASE(pBackBuffer);

	// Zバッファ用テクスチャ生成
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(td));
	td.Width = SCREEN_WIDTH;
	td.Height = SCREEN_HEIGHT;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	HRESULT hr = g_pDevice->CreateTexture2D(&td, nullptr, &g_pDepthStencilTexture);
	if (FAILED(hr)) {
		return hr;
	}

	// Zバッファターゲットビュー生成
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(dsvd));
	dsvd.Format = td.Format;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	hr = g_pDevice->CreateDepthStencilView(g_pDepthStencilTexture,
		&dsvd, &g_pDepthStencilView);
	if (FAILED(hr)) {
		return hr;
	}

	// 各ターゲットビューをレンダーターゲットに設定
	g_pDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

	// ビューポート設定
	D3D11_VIEWPORT vp;
	vp.Width = (float)SCREEN_WIDTH;
	vp.Height = (float)SCREEN_HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_pDeviceContext->RSSetViewports(1, &vp);

	return S_OK;
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT Init(HWND hWnd, BOOL bWindow)
{
	HRESULT hr = S_OK;

	// デバイス、スワップチェーンの作成
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(scd));
	scd.BufferCount = 1;
	scd.BufferDesc.Width = SCREEN_WIDTH;
	scd.BufferDesc.Height = SCREEN_HEIGHT;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = hWnd;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.Windowed = bWindow;

	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};

	hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
		nullptr, 0, featureLevels, _countof(featureLevels), D3D11_SDK_VERSION, &scd,
		&g_pSwapChain, &g_pDevice, nullptr, &g_pDeviceContext);
	if (FAILED(hr)) {
		return hr;
	}

	// バックバッファ生成
	hr = CreateBackBuffer();
	if (FAILED(hr)) {
		return hr;
	}

	// ラスタライズ設定
	D3D11_RASTERIZER_DESC rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_NONE;	// カリング無し(両面描画)
	g_pDevice->CreateRasterizerState(&rd, &g_pRs[0]);
	rd.CullMode = D3D11_CULL_FRONT;	// 前面カリング(裏面描画)
	g_pDevice->CreateRasterizerState(&rd, &g_pRs[1]);
	rd.CullMode = D3D11_CULL_BACK;	// 背面カリング(表面描画)
	g_pDevice->CreateRasterizerState(&rd, &g_pRs[2]);
	g_pDeviceContext->RSSetState(g_pRs[2]);

	// ブレンド ステート生成
	D3D11_BLEND_DESC BlendDesc;
	ZeroMemory(&BlendDesc, sizeof(BlendDesc));
	BlendDesc.AlphaToCoverageEnable = FALSE;
	BlendDesc.IndependentBlendEnable = FALSE;
	BlendDesc.RenderTarget[0].BlendEnable = FALSE;
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	g_pDevice->CreateBlendState(&BlendDesc, &g_pBlendState[0]);
	// ブレンド ステート生成 (アルファ ブレンド用)
	//BlendDesc.AlphaToCoverageEnable = TRUE;
	BlendDesc.RenderTarget[0].BlendEnable = TRUE;
	g_pDevice->CreateBlendState(&BlendDesc, &g_pBlendState[1]);
	// ブレンド ステート生成 (加算合成用)
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	g_pDevice->CreateBlendState(&BlendDesc, &g_pBlendState[2]);
	// ブレンド ステート生成 (減算合成用)
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
	g_pDevice->CreateBlendState(&BlendDesc, &g_pBlendState[3]);
	SetBlendState(BS_ALPHABLEND);

	// 深度ステンシルステート生成
	CD3D11_DEFAULT def;
	CD3D11_DEPTH_STENCIL_DESC dsd(def);
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	g_pDevice->CreateDepthStencilState(&dsd, &g_pDSS[0]);
	CD3D11_DEPTH_STENCIL_DESC dsd2(def);
	dsd2.DepthEnable = FALSE;
	g_pDevice->CreateDepthStencilState(&dsd2, &g_pDSS[1]);


	// サウンド初期化
	CSound::Init();
	CSound::SetVolume(1.0f);

	// ポリゴン表示初期化
	hr = InitPolygon(g_pDevice);
	if (FAILED(hr))
		return hr;

	// デバッグ文字列表示初期化
	hr = InitDebugProc();
	if (FAILED(hr))
		return hr;

	// 入力処理初期化
	hr = InitInput();
	if (FAILED(hr))
		return hr;

	// Assimp用シェーダ初期化
	if (!CAssimpModel::InitShader(g_pDevice))
		return E_FAIL;

	// メッシュ初期化
	hr = InitMesh();
	if (FAILED(hr))
		return hr;
	SetMeshCamera(&g_camera);
	SetMeshLight(&g_light);

	// ライト
	g_light.Init();
	CLight::SetMainLight(&g_light);

	// カメラ
	g_camera.Init();
	CCamera::SetMainCamera(&g_camera);


	// オブジェクト
	ECS::ObjectManager::Create();
	
	// システムを追加
	g_world.AddSystem<TransformSystem>();
	g_world.AddSystem<RendererSystem>();
	g_world.AddSystem<CollisionSystem>();
	g_world.AddSystem<RigidbodySystem>();
	g_world.AddSystem<ScriptSystem>();


	// ゲームオブジェクトを追加

	// プレイヤー
	const auto& player = g_world.GetEntityManager()->CreateEntity<GameObject>();
	player->AddComponent<PlayerScript>();
	

	for (int i = 0; i < 6; i++)
	{
		Vector3 pos;
		Vector3 scale;

		// 床
		if (i == 0)
		{
			pos = Vector3{ 5000, 0, 5000 };
			scale = Vector3{ 10000, 1000, 10000 };
		}
		// 天井
		else if (i == 1)
		{
			pos = Vector3{ 5000, 10000, 5000 };
			scale = Vector3{ 10000, 1000, 10000 };
		}
		// 右
		else if (i == 2)
		{
			pos = Vector3{ 10000, 5000, 5000 };
			scale = Vector3{ 1000, 10000, 10000 };
		}
		// 左
		else if (i == 3)
		{
			pos = Vector3{ 0, 5000, 5000 };
			scale = Vector3{ 1000, 10000, 10000 };
		}
		// 奥
		else if (i == 4)
		{
			pos = Vector3{ 5000, 5000, 10000 };
			scale = Vector3{ 10000, 10000, 1000 };
		}
		// 手前
		else if (i == 5)
		{
			pos = Vector3{ 5000, 5000, 0 };
			scale = Vector3{ 10000, 10000, 1000 };
		}

		const auto& test = g_world.GetEntityManager()->CreateEntity<GameObject>();
		const auto renderer = test->AddComponent<MeshRenderer>();
		renderer->MakeCube("plane");
		renderer->SetDiffuseTexture("data/texture/grid.png");
		renderer->SetTexSize({ 100, 100, 0 });
		renderer->UpdateTexMatrix();
		const auto& rb1 = test->AddComponent<Rigidbody>();
		test->AddComponent<BoxCollider>();
		test->transform().lock()->m_scale = scale;
		test->transform().lock()->m_pos = pos;
		rb1->SetUsePhysics(false);
		rb1->SetUseGravity(false);
		rb1->SetMass(100);
		rb1->SetE(0.8f);
		//rb1->SetDynamicFriction(0.0f);
	}

	Vector3 pos = { 400, 2000, 200 };
	//for (int i = 0; i < 10; i++)
	//{
	//	const auto& test = g_world.GetEntityManager()->CreateEntity<GameObject>();
	//	const auto& rb = test->AddComponent<Rigidbody>();
	//	//test->AddComponent<PlayerScript>();
	//	//test->AddComponent<MeshRenderer>()->MakeSphere("test", 100, 100);
	//	//test->AddComponent<SphereCollider>()->SetRadius(100);
	//	test->AddComponent<MeshRenderer>()->MakeCube("test");
	//	test->AddComponent<BoxCollider>();

	//	pos->y -= 300;
	//	test->transform().lock()->m_pos = pos;
	//	test->transform().lock()->m_scale = Vector3{ 100, 100, 100 };

	//	rb->SetMass(2);
	//	//rb->SetUsePhysics(false);
	//}

	pos = Vector3{ 2000, 2000,2000 };
	for (int z = 0; z < 3; z++)
	{
		for (int i = 0; i < 3; i++)
		{
			const auto& test = g_world.GetEntityManager()->CreateEntity<GameObject>();
			test->AddComponent<MeshRenderer>()->MakeSphere("test2");
			test->AddComponent<Rigidbody>();
			test->AddComponent<SphereCollider>();

			test->transform().lock()->m_pos = pos;
			test->transform().lock()->m_scale = Vector3{ 400, 400, 400 };
			pos->x += 2000;
		}
		pos->x = 1000;
		pos->z += 2000;
	}

	return hr;
}

//=============================================================================
// バックバッファ解放
//=============================================================================
void ReleaseBackBuffer()
{
	if (g_pDeviceContext) {
		g_pDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
	}
	SAFE_RELEASE(g_pDepthStencilView);
	SAFE_RELEASE(g_pDepthStencilTexture);
	SAFE_RELEASE(g_pRenderTargetView);
}

//=============================================================================
// 終了処理
//=============================================================================
void Uninit(void)
{
	// オブジェクト
	ECS::ObjectManager::Destroy();

	// メッシュ終了処理
	UninitMesh();

	// Assimp用シェーダ終了処理
	CAssimpModel::UninitShader();

	// 入力処理終了処理
	UninitInput();

	// デバッグ文字列表示終了処理
	UninitDebugProc();

	// ポリゴン表示終了処理
	UninitPolygon();

	// サウンド終了処理
	CSound::Fin();


	// 深度ステンシルステート解放
	for (int i = 0; i < _countof(g_pDSS); ++i) {
		SAFE_RELEASE(g_pDSS[i]);
	}

	// ブレンド ステート解放
	for (int i = 0; i < MAX_BLENDSTATE; ++i) {
		SAFE_RELEASE(g_pBlendState[i]);
	}

	// ラスタライザ ステート解放
	for (int i = 0; i < MAX_CULLMODE; ++i) {
		SAFE_RELEASE(g_pRs[i]);
	}

	// バックバッファ解放
	ReleaseBackBuffer();

	// スワップチェーン解放
	SAFE_RELEASE(g_pSwapChain);

	// デバイス コンテキストの開放
	SAFE_RELEASE(g_pDeviceContext);

	// デバイスの開放
	SAFE_RELEASE(g_pDevice);
}

//=============================================================================
// 更新処理
//=============================================================================
void Update(void)
{
	// 入力処理更新
	UpdateInput();	// 必ずUpdate関数の先頭で実行.

	// デバッグ文字列表示更新
	UpdateDebugProc();

	// デバッグ文字列設定
	StartDebugProc();
	PrintDebugProc("FPS:%d\n\n", g_nCountFPS);

	// ポリゴン表示更新
	UpdatePolygon();

	// ワールドの更新
	g_world.Update();

	// カメラ更新
	g_camera.Update();
	// ライトの更新
	g_light.Update();


	// サウンド更新処理
	CSound::Update();

	// オブジェクト
	ECS::ObjectManager::GetInstance()->ClearnUpObject();
}

//=============================================================================
// 描画処理
//=============================================================================
void Draw(void)
{
	// バックバッファ＆Ｚバッファのクリア
	float ClearColor[4] = { 0.117647f, 0.254902f, 0.352941f, 1.0f };
	g_pDeviceContext->ClearRenderTargetView(g_pRenderTargetView, ClearColor);
	g_pDeviceContext->ClearDepthStencilView(g_pDepthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	ClearShadowBuffer();

	// 各ターゲットビューをレンダーターゲットに設定
	g_pDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);


	// Zバッファ有効
	SetZBuffer(true);

	// ワールドの描画
	g_world.LateUpdate();

	// Zバッファ無効
	SetZBuffer(false);


//#ifdef _DEBUG
	// デバッグ文字列表示
	SetBlendState(BS_ALPHABLEND);
	SetPolygonColor(1.0f, 1.0f, 1.0f);
	DrawDebugProc();
	SetBlendState(BS_NONE);
//#endif // _DEBUG

	// バックバッファとフロントバッファの入れ替え
	g_pSwapChain->Present(g_uSyncInterval, 0);
}

//=============================================================================
// メイン ウィンドウ ハンドル取得
//=============================================================================
HWND GetMainWnd()
{
	return g_hWnd;
}

//=============================================================================
// インスタンス ハンドル取得
//=============================================================================
HINSTANCE GetInstance()
{
	return g_hInst;
}

//=============================================================================
// デバイス取得
//=============================================================================
ID3D11Device* GetDevice()
{
	return g_pDevice;
}

//=============================================================================
// デバイス コンテキスト取得
//=============================================================================
ID3D11DeviceContext* GetDeviceContext()
{
	return g_pDeviceContext;
}

//=============================================================================
// 深度バッファ有効無効制御
//=============================================================================
void SetZBuffer(bool bEnable)
{
	g_pDeviceContext->OMSetDepthStencilState((bEnable) ? nullptr : g_pDSS[1], 0);
}

//=============================================================================
// 深度バッファ更新有効無効制御
//=============================================================================
void SetZWrite(bool bEnable)
{
	g_pDeviceContext->OMSetDepthStencilState((bEnable) ? nullptr : g_pDSS[0], 0);
}

//=============================================================================
// ブレンド ステート設定
//=============================================================================
void SetBlendState(int nBlendState)
{
	if (nBlendState >= 0 && nBlendState < MAX_BLENDSTATE) {
		float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		g_pDeviceContext->OMSetBlendState(g_pBlendState[nBlendState], blendFactor, 0xffffffff);
	}
}

//=============================================================================
// カリング設定
//=============================================================================
void SetCullMode(int nCullMode)
{
	if (nCullMode >= 0 && nCullMode < MAX_CULLMODE) {
		g_pDeviceContext->RSSetState(g_pRs[nCullMode]);
	}
}


void SetRenderTarget()
{
	// 各ターゲットビューをレンダーターゲットに設定
	g_pDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);
}