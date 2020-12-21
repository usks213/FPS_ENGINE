//#define D3DCOMPILER

#include <stdio.h>
#include "Shader.h"
#include "../main.h"

#ifdef D3DCOMPILER
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler")
#endif

//---------------------------------------------------------------------------------------
// シェーダ読み込み
//---------------------------------------------------------------------------------------
static LPCWSTR RT_SHADER = L"SHADER";
static LPCWSTR g_pszCSODir = L"data\\shader\\";
HRESULT LoadVertexShader(LPCWSTR pwszVSFName, ID3D11VertexShader** ppVertexShader,
	ID3D11InputLayout** ppVertexLayout,
	const D3D11_INPUT_ELEMENT_DESC* pInpElemDesc, UINT uNumElem)
{
	WCHAR wszPath[_MAX_PATH], wszDrive[_MAX_DRIVE],
		wszDir[_MAX_DIR], wszFName[_MAX_FNAME], wszCSODir[_MAX_DIR];
	HRESULT hr = S_OK;
	ID3DBlob* pCompiledShader = nullptr;
	PBYTE pbData = nullptr;
	long lSize = 0L;

	// シェーダバイナリ読み込み
	HINSTANCE hInst = GetInstance();
	HWND hWnd = GetMainWnd();
	if (IS_INTRESOURCE(pwszVSFName)) {
		HRSRC hResInfo = FindResourceW(hInst, pwszVSFName, RT_SHADER);
		if (hResInfo) {
			HGLOBAL hResData = LoadResource(hInst, hResInfo);
			if (hResData) {
				lSize = SizeofResource(hInst, hResInfo);
				LPVOID pMem = LockResource(hResData);
				if (pMem) {
					pbData = new BYTE[lSize];
					CopyMemory(pbData, pMem, lSize);
					//UnlockResource(hResData);
				}
			}
		}
		if (!pbData) {
			WCHAR wszMsg[_MAX_PATH * 2];
			swprintf_s(wszMsg, _countof(wszMsg), L"頂点シェーダ(ID=%d) 読込エラー", (int)(ULONG_PTR)pwszVSFName);
			MessageBoxW(hWnd, wszMsg, L"error", MB_OK);
			return E_FAIL;
		}
	} else {
		_wsplitpath_s(pwszVSFName, wszDrive, _countof(wszDrive),
			wszDir, _countof(wszDir), wszFName, _countof(wszFName), nullptr, 0);
		wcscpy_s(wszCSODir, _countof(wszCSODir), wszDir);
		wcscat_s(wszCSODir, _countof(wszCSODir), g_pszCSODir);
		_wmakepath_s(wszPath, _countof(wszPath), wszDrive, wszCSODir, wszFName, L".cso");
		FILE* fp;
		_wfopen_s(&fp, wszPath, L"rb");
		if (fp) {
			fseek(fp, 0L, SEEK_END);
			lSize = ftell(fp);
			fseek(fp, 0L, SEEK_SET);
			pbData = new BYTE[lSize];
			fread(pbData, lSize, 1, fp);
			fclose(fp);
		} else {
#ifdef D3DCOMPILER
			// hlslファイル読み込み、ブロブ作成
			_wmakepath_s(wszPath, _countof(wszPath), wszDrive, wszDir, wszFName, L".hlsl");
			hr = D3DCompileFromFile(wszPath, nullptr, nullptr,
				"main", "vs_5_0", 0, 0, &pCompiledShader, nullptr);
			if (FAILED(hr)) {
				WCHAR wszMsg[_MAX_PATH * 2];
				swprintf(wszMsg, _countof(wszMsg), L"頂点シェーダ(%s) コンパイル失敗", pwszVSFName);
				MessageBoxW(hWnd, wszMsg, L"error", MB_OK);
				return hr;
			}
			pbData = (PBYTE)pCompiledShader->GetBufferPointer();
			lSize = (LONG)pCompiledShader->GetBufferSize();
#else
			WCHAR wszMsg[_MAX_PATH * 2];
			swprintf(wszMsg, _countof(wszMsg), L"頂点シェーダ(%s) 読み込みエラー", pwszVSFName);
			MessageBoxW(hWnd, wszMsg, L"error", MB_OK);
			return E_FAIL;
#endif
		}
	}
	// ブロブからバーテックスシェーダ作成
	ID3D11Device* pDevice = GetDevice();
	hr = pDevice->CreateVertexShader(pbData, lSize, nullptr, ppVertexShader);
	if (FAILED(hr)) {
		if (pCompiledShader)
			pCompiledShader->Release();
		else
			delete[] pbData;
		WCHAR wszMsg[_MAX_PATH * 2];
		swprintf_s(wszMsg, _countof(wszMsg), L"頂点シェーダ(%s) 生成失敗", pwszVSFName);
		MessageBoxW(hWnd, wszMsg, L"error", MB_OK);
		return hr;
	}

	// 頂点フォーマットの定義、生成
	static const D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION",    0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BONE_INDEX",  0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BONE_WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	if (!pInpElemDesc) {
		pInpElemDesc = layout;
		uNumElem = _countof(layout);
	}
	hr = pDevice->CreateInputLayout(pInpElemDesc, uNumElem, pbData, lSize, ppVertexLayout);
	if (FAILED(hr)) {
		if (pCompiledShader)
			pCompiledShader->Release();
		else
			delete[] pbData;
		WCHAR wszMsg[_MAX_PATH * 2];
		swprintf_s(wszMsg, _countof(wszMsg), L"頂点フォーマット生成失敗(%s)", pwszVSFName);
		MessageBoxW(hWnd, wszMsg, L"error", MB_OK);
		return hr;
	}
	if (pCompiledShader)
		pCompiledShader->Release();
	else
		delete[] pbData;
	return hr;
}

HRESULT LoadPixelShader(LPCWSTR pwszPSFName, ID3D11PixelShader** ppPixelShader)
{
	WCHAR wszPath[_MAX_PATH], wszDrive[_MAX_DRIVE],
		wszDir[_MAX_DIR], wszFName[_MAX_FNAME], wszCSODir[_MAX_DIR];
	HRESULT hr = S_OK;
	ID3DBlob* pCompiledShader = nullptr;
	PBYTE pbData = nullptr;
	long lSize = 0L;
	HINSTANCE hInst = GetInstance();
	HWND hWnd = GetMainWnd();

	// シェーダバイナリ読み込み
	if (IS_INTRESOURCE(pwszPSFName)) {
		HRSRC hResInfo = FindResourceW(hInst, pwszPSFName, RT_SHADER);
		if (hResInfo) {
			HGLOBAL hResData = LoadResource(hInst, hResInfo);
			if (hResData) {
				lSize = SizeofResource(hInst, hResInfo);
				LPVOID pMem = LockResource(hResData);
				if (pMem) {
					pbData = new BYTE[lSize];
					CopyMemory(pbData, pMem, lSize);
					//UnlockResource(hResData);
				}
			}
		}
		if (!pbData) {
			WCHAR wszMsg[_MAX_PATH * 2];
			swprintf_s(wszMsg, _countof(wszMsg), L"ピクセルシェーダ(ID=%d) 読込エラー", (int)(ULONG_PTR)pwszPSFName);
			MessageBoxW(hWnd, wszMsg, L"error", MB_OK);
			return E_FAIL;
		}
	} else {
		_wsplitpath_s(pwszPSFName, wszDrive, _countof(wszDrive),
			wszDir, _countof(wszDir), wszFName, _countof(wszFName), nullptr, 0);
		wcscpy_s(wszCSODir, _countof(wszCSODir), wszDir);
		wcscat_s(wszCSODir, _countof(wszCSODir), g_pszCSODir);
		_wmakepath_s(wszPath, _countof(wszPath), wszDrive, wszCSODir, wszFName, L".cso");
		FILE* fp = nullptr;
		_wfopen_s(&fp, wszPath, L"rb");
		if (fp) {
			fseek(fp, 0L, SEEK_END);
			lSize = ftell(fp);
			fseek(fp, 0L, SEEK_SET);
			pbData = new BYTE[lSize];
			fread(pbData, lSize, 1, fp);
			fclose(fp);
		} else {
#ifdef D3DCOMPILER
			// ブロブからピクセルシェーダ作成
			_wmakepath_s(wszPath, _countof(wszPath), wszDrive, wszDir, wszFName, L".hlsl");
			hr = D3DCompileFromFile(wszPath, nullptr, nullptr,
				"main", "ps_5_0", 0, 0, &pCompiledShader, nullptr);
			if (FAILED(hr)) {
				WCHAR wszMsg[_MAX_PATH * 2];
				swprintf_s(wszMsg, _countof(wszMsg), L"ピクセルシェーダ(%s) コンパイル失敗", pwszPSFName);
				MessageBoxW(hWnd, wszMsg, L"error", MB_OK);
				return hr;
			}
			pbData = (PBYTE)pCompiledShader->GetBufferPointer();
			lSize = (LONG)pCompiledShader->GetBufferSize();
#else
			WCHAR wszMsg[_MAX_PATH * 2];
			swprintf_s(wszMsg, _countof(wszMsg), L"ピクセルシェーダ(%s) 読み込みエラー", pwszPSFName);
			MessageBoxW(hWnd, wszMsg, L"error", MB_OK);
			return E_FAIL;
#endif
		}
	}
	ID3D11Device* pDevice = GetDevice();
	hr = pDevice->CreatePixelShader(pbData, lSize, nullptr, ppPixelShader);
	if (pCompiledShader)
		pCompiledShader->Release();
	else
		delete[] pbData;
	if (FAILED(hr)) {
		WCHAR wszMsg[_MAX_PATH * 2];
		swprintf_s(wszMsg, _countof(wszMsg), L"ピクセルシェーダ(%s) 生成失敗", pwszPSFName);
		MessageBoxW(hWnd, wszMsg, L"error", MB_OK);
		return hr;
	}
	return hr;
}

HRESULT LoadShader(LPCWSTR pwszVSFName, LPCWSTR pwszPSFName,
	ID3D11VertexShader** ppVertexShader, ID3D11InputLayout** ppVertexLayout,
	ID3D11PixelShader** ppPixelShader,
	const D3D11_INPUT_ELEMENT_DESC* pInpElemDesc, UINT uNumElem)
{
	// 頂点シェーダ読み込み
	HRESULT hr = LoadVertexShader(pwszVSFName, ppVertexShader, ppVertexLayout, pInpElemDesc, uNumElem);
	if (FAILED(hr)) return hr;
	// ピクセルシェーダ読み込み
	return LoadPixelShader(pwszPSFName, ppPixelShader);
}

HRESULT LoadVertexShader(LPCSTR pszVSFName, ID3D11VertexShader** ppVertexShader,
	ID3D11InputLayout** ppVertexLayout,
	const D3D11_INPUT_ELEMENT_DESC* pInpElemDesc, UINT uNumElem)
{
	if (IS_INTRESOURCE(pszVSFName)) {
		return LoadVertexShader((LPCWSTR)pszVSFName, ppVertexShader, ppVertexLayout, pInpElemDesc, uNumElem);
	}
	WCHAR wszVSFName[_MAX_PATH];
	int nLen = MultiByteToWideChar(CP_ACP, 0, pszVSFName, lstrlenA(pszVSFName), wszVSFName, _countof(wszVSFName));
	if (nLen <= 0) return E_FAIL;
	wszVSFName[nLen] = L'\0';
	return LoadVertexShader(wszVSFName, ppVertexShader, ppVertexLayout, pInpElemDesc, uNumElem);
}

HRESULT LoadPixelShader(LPCSTR pszPSFName, ID3D11PixelShader** ppPixelShader)
{
	if (IS_INTRESOURCE(pszPSFName)) {
		return LoadPixelShader((LPCWSTR)pszPSFName, ppPixelShader);
	}
	WCHAR wszPSFName[_MAX_PATH];
	int nLen = MultiByteToWideChar(CP_ACP, 0, pszPSFName, lstrlenA(pszPSFName), wszPSFName, _countof(wszPSFName));
	if (nLen <= 0) return E_FAIL;
	wszPSFName[nLen] = L'\0';
	return LoadPixelShader(wszPSFName, ppPixelShader);
}

HRESULT LoadShader(LPCSTR pszVSFName, LPCSTR pszPSFName,
	ID3D11VertexShader** ppVertexShader, ID3D11InputLayout** ppVertexLayout,
	ID3D11PixelShader** ppPixelShader,
	const D3D11_INPUT_ELEMENT_DESC* pInpElemDesc, UINT uNumElem)
{
	HRESULT hr = LoadVertexShader(pszVSFName, ppVertexShader, ppVertexLayout, pInpElemDesc, uNumElem);
	if (FAILED(hr)) return hr;
	return LoadPixelShader(pszPSFName, ppPixelShader);
}
