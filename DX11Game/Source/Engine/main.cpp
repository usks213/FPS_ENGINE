//======================================================================
//											main.cpp
//	ECS�݌v�t���[�����[�N(�f�[�^�w������Ȃ��D�D�D)
//
//======================================================================
//	author :	AT12A 05 �F�����W�V
//======================================================================
//	�J������
//
//	2020/12/21	ECS�x�[�X�łP����J���X�^�[�g
//				Object,Entity,Component �̍쐬����
//
//	2020/12/22	World,EntityManager,System �̍쐬
//	
//	2020/12/23	Transform,TransformSystem �̍쐬
//				�V�X�e���ƃR���|�[�l���g�̂���肤�܂��������I�I
//				Renderer, RendererSystem �̍쐬
//
//	2020/12/26	Collider,CollisionSystem �̍쐬
//				BoxCollider,SphereCollider �̍쐬
//
//	2020/12/27	GameObject,Script,ScriptSystem �̍쐬
//				ScriptBaseLayout �̍쐬
//
//	2020/12/28	Rigidbody,RigidbodySystem �̍쐬
//				�����v�Z����(��肭�����Ȃ�)
//
//	2020/12/30	�����蔻��C�� �����v�Z��肭�������I�I
//
//	2020/12/31	FPS���_�쐬�A�����e�����Ȃ�
//
//	2021/01/01	�����܂��Ă��߂łƂ��������܂��I�I
//
//	2021/01/02	Animator2D,Animation2D,Animation2DSystem �̍쐬
//				SpriteRenderer,BillboardRenderer,AssimpRenderer �̍쐬
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

// �V�X�e��
#include "ECSSystem/TransformSystem.h"
#include "ECSSystem/RendererSystem.h"
#include "ECSSystem/CollisionSystem.h"
#include "ECSSystem/RigidbodySystem.h"
#include "ECSSystem/ScriptSystem.h"

// �R���|�[�l���g
#include "ECSCompoent/Transform.h"
#include "ECSCompoent/MeshRenderer.h"
#include "ECSCompoent/Rigidbody.h"
#include "ECSCompoent/SphereCollider.h"
#include "ECSCompoent/BoxCollider.h"

// �����_���[
#include "Renderer/Camera.h"
#include "Renderer/Light.h"
#include "Renderer/mesh.h"
#include "Renderer/AssimpModel.h"

// �X�N���v�g
#include "../Scripts/PlayerScript.h"



//-------- ���C�u�����̃����N
#pragma comment(lib, "winmm")
#pragma comment(lib, "imm32")
#pragma comment(lib, "d3d11")

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define CLASS_NAME		_T("AppClass")					// �E�C���h�E�̃N���X��
#define WINDOW_NAME		_T("���[�^����")			// �E�C���h�E�̃L���v�V������

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int OnCreate(HWND hWnd, LPCREATESTRUCT lpcs);
HRESULT Init(HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);

//*****************************************************************************
// �O���[�o���ϐ�:
//*****************************************************************************
HWND						g_hWnd;					// ���C�� �E�B���h�E �n���h��
HINSTANCE					g_hInst;				// �C���X�^���X �n���h��

ID3D11Device*				g_pDevice;				// �f�o�C�X
ID3D11DeviceContext*		g_pDeviceContext;		// �f�o�C�X �R���e�L�X�g
IDXGISwapChain*				g_pSwapChain;			// �X���b�v�`�F�[��
ID3D11RenderTargetView*		g_pRenderTargetView;	// �t���[���o�b�t�@
ID3D11Texture2D*			g_pDepthStencilTexture;	// Z�o�b�t�@�p������
ID3D11DepthStencilView*		g_pDepthStencilView;	// Z�o�b�t�@
UINT						g_uSyncInterval = 1;	// �������� (0:��, 1:�L)
ID3D11RasterizerState*		g_pRs[MAX_CULLMODE];	// ���X�^���C�U �X�e�[�g
ID3D11BlendState*			g_pBlendState[MAX_BLENDSTATE];// �u�����h �X�e�[�g
ID3D11DepthStencilState*	g_pDSS[2];				// Z/�X�e���V�� �X�e�[�g

int							g_nCountFPS;			// FPS�J�E���^

CCamera						g_camera;				// �J����
CLight						g_light;				// ����

using namespace ECS;

World g_world;


//=============================================================================
// ���C���֐�
//=============================================================================
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// ���g�p�錾
	UNREFERENCED_PARAMETER(lpCmdLine);		// ���g�p�錾

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
	
	// COM������
	if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED))) {
		MessageBox(NULL, _T("COM�̏������Ɏ��s���܂����B"), _T("error"), MB_OK);
		return -1;
	}

	// �C���X�^���X �n���h���ۑ�
	g_hInst = hInstance;

	// �E�B���h�E�N���X�̓o�^
	RegisterClassEx(&wcex);

	// �N���C�A���g�̈�T�C�Y����E�B���h�E �T�C�Y�Z�o
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION
		| WS_SYSMENU | WS_BORDER | WS_MINIMIZEBOX;
	DWORD dwExStyle = 0;
	RECT rc = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	AdjustWindowRectEx(&rc, dwStyle, FALSE, dwExStyle);

	// �E�B���h�E�̍쐬
	g_hWnd = CreateWindowEx(dwExStyle,
		CLASS_NAME,
		WINDOW_NAME,
		dwStyle,
		CW_USEDEFAULT,		// �E�B���h�E�̍����W
		CW_USEDEFAULT,		// �E�B���h�E�̏���W
		rc.right - rc.left,	// �E�B���h�E����
		rc.bottom - rc.top,	// �E�B���h�E�c��
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	// �t���[���J�E���g������
	timeBeginPeriod(1);				// ����\��ݒ�
	dwExecLastTime = dwFPSLastTime = timeGetTime();
	dwCurrentTime = dwFrameCount = 0;

	// �E�C���h�E�̕\��(�����������̌�ɌĂ΂Ȃ��Ƒʖ�)
	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);
	
	// DirectX�̏�����(�E�B���h�E���쐬���Ă���s��)
	if (FAILED(Init(g_hWnd, true))) {
		return -1;
	}

	// ���b�Z�[�W���[�v
	for (;;) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				// PostQuitMessage()���Ă΂ꂽ�烋�[�v�I��
				break;
			} else {
				// ���b�Z�[�W�̖|��ƃf�B�X�p�b�`
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		} else {
			dwCurrentTime = timeGetTime();
			if ((dwCurrentTime - dwFPSLastTime) >= 500) {	// 0.5�b���ƂɎ��s
				g_nCountFPS = dwFrameCount * 1000 / (dwCurrentTime - dwFPSLastTime);
				dwFPSLastTime = dwCurrentTime;
				dwFrameCount = 0;
			}
			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60)) {
				dwExecLastTime = dwCurrentTime;
				// �X�V����
				Update();
			}
			// �`�揈��
			Draw();
			dwFrameCount++;
		}
	}

	// �^�C�}�ݒ�����ɖ߂�
	timeEndPeriod(1);

	// �I������
	Uninit();

	// �E�B���h�E�N���X�̓o�^������
	UnregisterClass(CLASS_NAME, g_hInst);

	// COM�I������
	CoUninitialize();

	return (int)msg.wParam;
}

//=============================================================================
// �v���V�[�W��
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:					//----- �E�B���h�E���������ꂽ
		return OnCreate(hWnd, (LPCREATESTRUCT)lParam);
	case WM_DESTROY:				//----- �E�B���h�E�j���w��������
		PostQuitMessage(0);				// �V�X�e���ɃX���b�h�̏I����v��
		break;
	case WM_KEYDOWN:				//----- �L�[�{�[�h�������ꂽ
		switch (wParam) {
		case VK_ESCAPE:					// [ESC]�L�[�������ꂽ
			PostMessage(hWnd, WM_CLOSE, 0, 0);	// [x]�������ꂽ�悤�ɐU����
			return 0;
		}
		break;
	case WM_MENUCHAR:
		return MNC_CLOSE << 16;			// [Alt]+[Enter]����BEEP��}�~
	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//=============================================================================
// WM_CREATE���b�Z�[�W�n���h��
//=============================================================================
int OnCreate(HWND hWnd, LPCREATESTRUCT lpcs)
{
	// �N���C�A���g�̈�T�C�Y��SCREEN_WIDTH�~SCREEN_HEIGHT�ɍĐݒ�.
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

	// IME������
	ImmAssociateContext(hWnd, nullptr);

	return 0;	// -1��Ԃ���CreateWindow[Ex]�����s����.
}

//=============================================================================
// �o�b�N�o�b�t�@����
//=============================================================================
HRESULT CreateBackBuffer(void)
{
	ID3D11Device* pDevice = g_pDevice;

	// �����_�[�^�[�Q�b�g�r���[����
	ID3D11Texture2D* pBackBuffer = nullptr;
	g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	g_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
	SAFE_RELEASE(pBackBuffer);

	// Z�o�b�t�@�p�e�N�X�`������
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

	// Z�o�b�t�@�^�[�Q�b�g�r���[����
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(dsvd));
	dsvd.Format = td.Format;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	hr = g_pDevice->CreateDepthStencilView(g_pDepthStencilTexture,
		&dsvd, &g_pDepthStencilView);
	if (FAILED(hr)) {
		return hr;
	}

	// �e�^�[�Q�b�g�r���[�������_�[�^�[�Q�b�g�ɐݒ�
	g_pDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

	// �r���[�|�[�g�ݒ�
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
// ����������
//=============================================================================
HRESULT Init(HWND hWnd, BOOL bWindow)
{
	HRESULT hr = S_OK;

	// �f�o�C�X�A�X���b�v�`�F�[���̍쐬
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

	// �o�b�N�o�b�t�@����
	hr = CreateBackBuffer();
	if (FAILED(hr)) {
		return hr;
	}

	// ���X�^���C�Y�ݒ�
	D3D11_RASTERIZER_DESC rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_NONE;	// �J�����O����(���ʕ`��)
	g_pDevice->CreateRasterizerState(&rd, &g_pRs[0]);
	rd.CullMode = D3D11_CULL_FRONT;	// �O�ʃJ�����O(���ʕ`��)
	g_pDevice->CreateRasterizerState(&rd, &g_pRs[1]);
	rd.CullMode = D3D11_CULL_BACK;	// �w�ʃJ�����O(�\�ʕ`��)
	g_pDevice->CreateRasterizerState(&rd, &g_pRs[2]);
	g_pDeviceContext->RSSetState(g_pRs[2]);

	// �u�����h �X�e�[�g����
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
	// �u�����h �X�e�[�g���� (�A���t�@ �u�����h�p)
	//BlendDesc.AlphaToCoverageEnable = TRUE;
	BlendDesc.RenderTarget[0].BlendEnable = TRUE;
	g_pDevice->CreateBlendState(&BlendDesc, &g_pBlendState[1]);
	// �u�����h �X�e�[�g���� (���Z�����p)
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	g_pDevice->CreateBlendState(&BlendDesc, &g_pBlendState[2]);
	// �u�����h �X�e�[�g���� (���Z�����p)
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
	g_pDevice->CreateBlendState(&BlendDesc, &g_pBlendState[3]);
	SetBlendState(BS_ALPHABLEND);

	// �[�x�X�e���V���X�e�[�g����
	CD3D11_DEFAULT def;
	CD3D11_DEPTH_STENCIL_DESC dsd(def);
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	g_pDevice->CreateDepthStencilState(&dsd, &g_pDSS[0]);
	CD3D11_DEPTH_STENCIL_DESC dsd2(def);
	dsd2.DepthEnable = FALSE;
	g_pDevice->CreateDepthStencilState(&dsd2, &g_pDSS[1]);


	// �T�E���h������
	CSound::Init();
	CSound::SetVolume(1.0f);

	// �|���S���\��������
	hr = InitPolygon(g_pDevice);
	if (FAILED(hr))
		return hr;

	// �f�o�b�O������\��������
	hr = InitDebugProc();
	if (FAILED(hr))
		return hr;

	// ���͏���������
	hr = InitInput();
	if (FAILED(hr))
		return hr;

	// Assimp�p�V�F�[�_������
	if (!CAssimpModel::InitShader(g_pDevice))
		return E_FAIL;

	// ���b�V��������
	hr = InitMesh();
	if (FAILED(hr))
		return hr;
	SetMeshCamera(&g_camera);
	SetMeshLight(&g_light);

	// ���C�g
	g_light.Init();
	CLight::SetMainLight(&g_light);

	// �J����
	g_camera.Init();
	CCamera::SetMainCamera(&g_camera);


	// �I�u�W�F�N�g
	ECS::ObjectManager::Create();
	
	// �V�X�e����ǉ�
	g_world.AddSystem<TransformSystem>();
	g_world.AddSystem<RendererSystem>();
	g_world.AddSystem<CollisionSystem>();
	g_world.AddSystem<RigidbodySystem>();
	g_world.AddSystem<ScriptSystem>();


	// �Q�[���I�u�W�F�N�g��ǉ�

	// �v���C���[
	const auto& player = g_world.GetEntityManager()->CreateEntity<GameObject>();
	player->AddComponent<PlayerScript>();
	

	for (int i = 0; i < 6; i++)
	{
		Vector3 pos;
		Vector3 scale;

		// ��
		if (i == 0)
		{
			pos = Vector3{ 5000, 0, 5000 };
			scale = Vector3{ 10000, 1000, 10000 };
		}
		// �V��
		else if (i == 1)
		{
			pos = Vector3{ 5000, 10000, 5000 };
			scale = Vector3{ 10000, 1000, 10000 };
		}
		// �E
		else if (i == 2)
		{
			pos = Vector3{ 10000, 5000, 5000 };
			scale = Vector3{ 1000, 10000, 10000 };
		}
		// ��
		else if (i == 3)
		{
			pos = Vector3{ 0, 5000, 5000 };
			scale = Vector3{ 1000, 10000, 10000 };
		}
		// ��
		else if (i == 4)
		{
			pos = Vector3{ 5000, 5000, 10000 };
			scale = Vector3{ 10000, 10000, 1000 };
		}
		// ��O
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
// �o�b�N�o�b�t�@���
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
// �I������
//=============================================================================
void Uninit(void)
{
	// �I�u�W�F�N�g
	ECS::ObjectManager::Destroy();

	// ���b�V���I������
	UninitMesh();

	// Assimp�p�V�F�[�_�I������
	CAssimpModel::UninitShader();

	// ���͏����I������
	UninitInput();

	// �f�o�b�O������\���I������
	UninitDebugProc();

	// �|���S���\���I������
	UninitPolygon();

	// �T�E���h�I������
	CSound::Fin();


	// �[�x�X�e���V���X�e�[�g���
	for (int i = 0; i < _countof(g_pDSS); ++i) {
		SAFE_RELEASE(g_pDSS[i]);
	}

	// �u�����h �X�e�[�g���
	for (int i = 0; i < MAX_BLENDSTATE; ++i) {
		SAFE_RELEASE(g_pBlendState[i]);
	}

	// ���X�^���C�U �X�e�[�g���
	for (int i = 0; i < MAX_CULLMODE; ++i) {
		SAFE_RELEASE(g_pRs[i]);
	}

	// �o�b�N�o�b�t�@���
	ReleaseBackBuffer();

	// �X���b�v�`�F�[�����
	SAFE_RELEASE(g_pSwapChain);

	// �f�o�C�X �R���e�L�X�g�̊J��
	SAFE_RELEASE(g_pDeviceContext);

	// �f�o�C�X�̊J��
	SAFE_RELEASE(g_pDevice);
}

//=============================================================================
// �X�V����
//=============================================================================
void Update(void)
{
	// ���͏����X�V
	UpdateInput();	// �K��Update�֐��̐擪�Ŏ��s.

	// �f�o�b�O������\���X�V
	UpdateDebugProc();

	// �f�o�b�O������ݒ�
	StartDebugProc();
	PrintDebugProc("FPS:%d\n\n", g_nCountFPS);

	// �|���S���\���X�V
	UpdatePolygon();

	// ���[���h�̍X�V
	g_world.Update();

	// �J�����X�V
	g_camera.Update();
	// ���C�g�̍X�V
	g_light.Update();


	// �T�E���h�X�V����
	CSound::Update();

	// �I�u�W�F�N�g
	ECS::ObjectManager::GetInstance()->ClearnUpObject();
}

//=============================================================================
// �`�揈��
//=============================================================================
void Draw(void)
{
	// �o�b�N�o�b�t�@���y�o�b�t�@�̃N���A
	float ClearColor[4] = { 0.117647f, 0.254902f, 0.352941f, 1.0f };
	g_pDeviceContext->ClearRenderTargetView(g_pRenderTargetView, ClearColor);
	g_pDeviceContext->ClearDepthStencilView(g_pDepthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	ClearShadowBuffer();

	// �e�^�[�Q�b�g�r���[�������_�[�^�[�Q�b�g�ɐݒ�
	g_pDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);


	// Z�o�b�t�@�L��
	SetZBuffer(true);

	// ���[���h�̕`��
	g_world.LateUpdate();

	// Z�o�b�t�@����
	SetZBuffer(false);


//#ifdef _DEBUG
	// �f�o�b�O������\��
	SetBlendState(BS_ALPHABLEND);
	SetPolygonColor(1.0f, 1.0f, 1.0f);
	DrawDebugProc();
	SetBlendState(BS_NONE);
//#endif // _DEBUG

	// �o�b�N�o�b�t�@�ƃt�����g�o�b�t�@�̓���ւ�
	g_pSwapChain->Present(g_uSyncInterval, 0);
}

//=============================================================================
// ���C�� �E�B���h�E �n���h���擾
//=============================================================================
HWND GetMainWnd()
{
	return g_hWnd;
}

//=============================================================================
// �C���X�^���X �n���h���擾
//=============================================================================
HINSTANCE GetInstance()
{
	return g_hInst;
}

//=============================================================================
// �f�o�C�X�擾
//=============================================================================
ID3D11Device* GetDevice()
{
	return g_pDevice;
}

//=============================================================================
// �f�o�C�X �R���e�L�X�g�擾
//=============================================================================
ID3D11DeviceContext* GetDeviceContext()
{
	return g_pDeviceContext;
}

//=============================================================================
// �[�x�o�b�t�@�L����������
//=============================================================================
void SetZBuffer(bool bEnable)
{
	g_pDeviceContext->OMSetDepthStencilState((bEnable) ? nullptr : g_pDSS[1], 0);
}

//=============================================================================
// �[�x�o�b�t�@�X�V�L����������
//=============================================================================
void SetZWrite(bool bEnable)
{
	g_pDeviceContext->OMSetDepthStencilState((bEnable) ? nullptr : g_pDSS[0], 0);
}

//=============================================================================
// �u�����h �X�e�[�g�ݒ�
//=============================================================================
void SetBlendState(int nBlendState)
{
	if (nBlendState >= 0 && nBlendState < MAX_BLENDSTATE) {
		float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		g_pDeviceContext->OMSetBlendState(g_pBlendState[nBlendState], blendFactor, 0xffffffff);
	}
}

//=============================================================================
// �J�����O�ݒ�
//=============================================================================
void SetCullMode(int nCullMode)
{
	if (nCullMode >= 0 && nCullMode < MAX_CULLMODE) {
		g_pDeviceContext->RSSetState(g_pRs[nCullMode]);
	}
}


void SetRenderTarget()
{
	// �e�^�[�Q�b�g�r���[�������_�[�^�[�Q�b�g�ɐݒ�
	g_pDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);
}