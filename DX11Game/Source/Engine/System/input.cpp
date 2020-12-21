//=============================================================================
//
// 入力処理 [input.cpp]
// Author : KOSHI USAMI
//
//=============================================================================
#include "input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	NUM_KEY_MAX			(256)				// キー最大数
#define NUM_JOY_MAX			(32)				// ゲームパッド最大数

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
static HRESULT InitKeyboard();
static void UninitKeyboard();
static HRESULT UpdateKeyboard();

static HRESULT InitJoystick();
static void UninitJoystick();
static HRESULT UpdateJoystick();

static HRESULT InitMouse();
static void UninitMouse();
static HRESULT UpdateMouse();

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static WORD				g_wKeyState[NUM_KEY_MAX];			// キーボードの押下状態を保持するワーク
static WORD				g_wKeyStateTrigger[NUM_KEY_MAX];	// キーボードのトリガ状態を保持するワーク
static WORD				g_wKeyStateRelease[NUM_KEY_MAX];	// キーボードのリリース状態を保持するワーク
static WORD				g_wKeyStateRepeat[NUM_KEY_MAX];		// キーボードのリピート状態を保持するワーク
static int				g_nKeyStateRepeatCnt[NUM_KEY_MAX];	// キーボードのリピートカウンタ

static DWORD			g_dwJoyCount;
static JOYINFOEX		g_joyState[NUM_JOY_MAX];
static DWORD			g_dwJoyButtonTrigger[NUM_JOY_MAX];
static DWORD			g_dwJoyButtonRelease[NUM_JOY_MAX];

static POINT			g_ptMouse;
static const int		g_nMouseBtn[] = { VK_LBUTTON, VK_RBUTTON, VK_MBUTTON, VK_XBUTTON1, VK_XBUTTON2 };

//=============================================================================
// 入力処理の初期化
//=============================================================================
HRESULT InitInput()
{
	HRESULT hr = S_OK;

	// キーボードの初期化
	hr = InitKeyboard();

	// ゲームパッドの初期化
	if (SUCCEEDED(hr)) {
		hr = InitJoystick();
	}

	// マウスの初期化
	if (SUCCEEDED(hr)) {
		hr = InitMouse();
	}

	return hr;
}

//=============================================================================
// 入力処理の終了処理
//=============================================================================
void UninitInput()
{
	// マウスの終了処理
	UninitMouse();

	// ゲームパッドの終了処理
	UninitJoystick();

	// キーボードの終了処理
	UninitKeyboard();
}

//=============================================================================
// 入力処理の更新処理
//=============================================================================
void UpdateInput()
{
	// キーボードの更新
	UpdateKeyboard();

	// ゲームパッドの更新
	UpdateJoystick();

	// マウスの更新
	UpdateMouse();
}

//=============================================================================
// キーボードの初期化処理
//=============================================================================
HRESULT InitKeyboard()
{
	HRESULT hr = S_OK;

	return hr;
}

//=============================================================================
// キーボードの終了処理
//=============================================================================
void UninitKeyboard(void)
{
	// (何もしない)
}

//=============================================================================
// キーボードの更新処理
//=============================================================================
HRESULT UpdateKeyboard(void)
{
	HRESULT hr = S_OK;
	WORD wKeyState;

	// キーボードの状態を取得
	for (int nCntKey = 0; nCntKey < NUM_KEY_MAX; nCntKey++) {
		wKeyState = GetAsyncKeyState(nCntKey);

		g_wKeyStateTrigger[nCntKey] = (g_wKeyState[nCntKey] ^ wKeyState) & wKeyState;
		g_wKeyStateRelease[nCntKey] = (g_wKeyState[nCntKey] ^ wKeyState) & ~wKeyState;
		g_wKeyStateRepeat[nCntKey] = g_wKeyStateTrigger[nCntKey];

		if (wKeyState & 0x8000) {
			g_nKeyStateRepeatCnt[nCntKey]++;
			if (g_nKeyStateRepeatCnt[nCntKey] >= 20) {
				g_wKeyStateRepeat[nCntKey] = wKeyState;
			}
		} else {
			g_nKeyStateRepeatCnt[nCntKey] = 0;
			g_wKeyStateRepeat[nCntKey] = 0;
		}

		g_wKeyState[nCntKey] = wKeyState;
	}

	return hr;
}

//=============================================================================
// ゲームパッドの初期化処理
//=============================================================================
HRESULT InitJoystick()
{
	GetJoyCount();

	return S_OK;
}

//=============================================================================
// ゲームパッドの終了処理
//=============================================================================
void UninitJoystick()
{
	// (何もしない)
}

//=============================================================================
// ゲームパッドの更新処理
//=============================================================================
HRESULT UpdateJoystick()
{
	HRESULT hr = S_OK;

	for (DWORD i = 0; i < g_dwJoyCount; ++i) {
		JOYINFOEX& ji = g_joyState[i];
		DWORD dwButtons = ji.dwButtons;
		ZeroMemory(&ji, sizeof(ji));
		ji.dwSize = sizeof(ji);
		ji.dwFlags = JOY_RETURNALL;
		if (joyGetPosEx(i, &ji) != JOYERR_NOERROR) {
			hr = E_FAIL;
			break;
		}
		g_dwJoyButtonTrigger[i] = (g_joyState[i].dwButtons ^ dwButtons) & dwButtons;
		g_dwJoyButtonRelease[i] = (g_joyState[i].dwButtons ^ dwButtons) & ~dwButtons;
	}
	return hr;
}

//=============================================================================
// マウスの初期化処理
//=============================================================================
HRESULT InitMouse()
{
	return S_OK;
}

//=============================================================================
// マウスの終了処理
//=============================================================================
void UninitMouse()
{
	// (何もしない)
}

//=============================================================================
// マウスの更新処理
//=============================================================================
HRESULT UpdateMouse()
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(GetMainWnd(), &pt);
	g_ptMouse = pt;
	return S_OK;
}

//=============================================================================
// キーボードのプレス状態を取得
//=============================================================================
bool GetKeyPress(int nKey)
{
	return (g_wKeyState[nKey] & 0x8000) ? true : false;
}

//=============================================================================
// キーボードのトリガ状態を取得
//=============================================================================
bool GetKeyTrigger(int nKey)
{
	return (g_wKeyStateTrigger[nKey] & 0x8000) ? true : false;
}

//=============================================================================
// キーボードのリピート状態を取得
//=============================================================================
bool GetKeyRepeat(int nKey)
{
	return (g_wKeyStateRepeat[nKey] & 0x8000) ? true : false;
}

//=============================================================================
// キーボードのリリ－ス状態を取得
//=============================================================================
bool GetKeyRelease(int nKey)
{
	return (g_wKeyStateRelease[nKey] & 0x8000) ? true : false;
}

//=============================================================================
// ゲームパッド接続数取得
//=============================================================================
DWORD GetJoyCount()
{
	JOYINFO	ji;
	DWORD dwCount = joyGetNumDevs();
	if (dwCount > NUM_JOY_MAX)
		dwCount = NUM_JOY_MAX;
	for (g_dwJoyCount = 0; g_dwJoyCount < dwCount; ++g_dwJoyCount) {
		if (joyGetPos(g_dwJoyCount, &ji) != JOYERR_NOERROR)
			break;
	}
	return g_dwJoyCount;
}

//=============================================================================
// ゲームパッド情報取得
//=============================================================================
JOYINFOEX *GetJoyState(DWORD dwJoy)
{
	if (dwJoy >= NUM_JOY_MAX) return nullptr;
	return &g_joyState[dwJoy];
}

//=============================================================================
// ゲームパッドX軸取得
//=============================================================================
LONG GetJoyX(DWORD dwJoy)
{
	if (dwJoy >= NUM_JOY_MAX) return 0L;
	return (LONG)g_joyState[dwJoy].dwXpos - 0x08000;
}

//=============================================================================
// ゲームパッドY軸取得
//=============================================================================
LONG GetJoyY(DWORD dwJoy)
{
	if (dwJoy >= NUM_JOY_MAX) return 0L;
	return (LONG)g_joyState[dwJoy].dwYpos - 0x08000;
}

//=============================================================================
// ゲームパッドZ軸取得
//=============================================================================
LONG GetJoyZ(DWORD dwJoy)
{
	if (dwJoy >= NUM_JOY_MAX) return 0L;
	return (LONG)g_joyState[dwJoy].dwZpos - 0x08000;
}

//=============================================================================
// ゲームパッド ボタン情報取得
//=============================================================================
bool GetJoyButton(DWORD dwJoy, DWORD dwBtn)
{
	if (dwJoy >= NUM_JOY_MAX) return false;
	return (g_joyState[dwJoy].dwButtons & (1 << dwBtn)) ? true : false;
}

//=============================================================================
// ゲームパッド トリガ情報取得
//=============================================================================
bool GetJoyTrigger(DWORD dwJoy, DWORD dwBtn)
{
	if (dwJoy >= NUM_JOY_MAX) return false;
	return (g_dwJoyButtonTrigger[dwJoy] & (1 << dwBtn)) ? true : false;
}

//=============================================================================
// ゲームパッド リリース情報取得
//=============================================================================
bool GetJoyRelease(DWORD dwJoy, DWORD dwBtn)
{
	if (dwJoy >= NUM_JOY_MAX) return false;
	return (g_dwJoyButtonRelease[dwJoy] & (1 << dwBtn)) ? true : false;
}

//=============================================================================
// マウス座標取得
//=============================================================================
POINT *GetMousePosition()
{
	return &g_ptMouse;
}

//=============================================================================
// マウス ボタン情報取得
//=============================================================================
bool GetMouseButton(DWORD dwBtn)
{
	if (dwBtn >= _countof(g_nMouseBtn)) return false;
	return GetKeyPress(g_nMouseBtn[dwBtn]);
}

//=============================================================================
// マウス トリガ情報取得
//=============================================================================
bool GetMouseTrigger(DWORD dwBtn)
{
	if (dwBtn >= _countof(g_nMouseBtn)) return false;
	return GetKeyTrigger(g_nMouseBtn[dwBtn]);
}

//=============================================================================
// マウス リリース情報取得
//=============================================================================
bool GetMouseRelease(DWORD dwBtn)
{
	if (dwBtn >= _countof(g_nMouseBtn)) return false;
	return GetKeyRelease(g_nMouseBtn[dwBtn]);
}
