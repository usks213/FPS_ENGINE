//=============================================================================
//
// デバッグ表示処理 [debugproc.h]
// Author : KOSHI USAMI
//
//=============================================================================
#pragma once

#include "../main.h"

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitDebugProc(void);
void UninitDebugProc(void);
void UpdateDebugProc(void);
void DrawDebugProc(void);

void StartDebugProc(bool hiragana = false);
void PrintDebugProc(const char *fmt, ...);
bool SetHiragana(bool hiragana = true);
