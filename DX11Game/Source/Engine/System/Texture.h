#ifdef _MSC_VER
#pragma once
#endif

//#include "main.h"
#include <d3d11.h>
#include <DirectXTex.h>
#include <string>
#include <map>

#pragma warning(push)
#pragma warning(disable : 4005)
#include <stdint.h>
#pragma warning(pop)

HRESULT CreateTextureFromMemory(_In_ ID3D11Device* d3dDevice,
	_In_bytecount_(wicDataSize) const uint8_t* wicData,
	_In_ size_t wicDataSize,
	_Out_opt_ ID3D11ShaderResourceView** textureView,
	_Out_opt_ DirectX::TexMetadata* pTexInfo = nullptr
);



class CTexture
{
public:
	static ID3D11ShaderResourceView* GetTexture(ID3D11Device* d3dDevice, const char* pszFilename)
	{
		// �v�[�����猟��
		const auto& itr = m_texturePool_c.find(pszFilename);
		
		// ���Ƀv�[���ɂ�������
		if (m_texturePool_c.end() != itr)
		{
			return itr->second;
		}

		// �V�������[�h
		ID3D11ShaderResourceView* tex = nullptr;
		HRESULT hr = CreateTextureFromFile(d3dDevice, pszFilename, &tex);

		// �G���[����
		if (FAILED(hr))
		{
			//MessageBoxA(GetMainWnd(), "�e�N�X�`���ǂݍ��݃G���[", pszFilename, MB_OK);
			return nullptr;
		}
		
		// �i�[
		m_texturePool_c.emplace(pszFilename, tex);
		
		return tex;
	}

	static ID3D11ShaderResourceView* GetTexture(ID3D11Device* d3dDevice, const wchar_t* pszFilename)
	{
		// �v�[�����猟��
		const auto& itr = m_texturePool_w.find(pszFilename);

		// ���Ƀv�[���ɂ�������
		if (m_texturePool_w.end() != itr)
		{
			return itr->second;
		}

		// �V�������[�h
		ID3D11ShaderResourceView* tex = nullptr;
		HRESULT hr = CreateTextureFromFile(d3dDevice, pszFilename, &tex);

		// �G���[����
		if (FAILED(hr))
		{
			//MessageBox(GetMainWnd(), L"�e�N�X�`���ǂݍ��݃G���[", pszFilename, MB_OK);
			return nullptr;
		}

		// �i�[
		m_texturePool_w.emplace(pszFilename, tex);

		return tex;
	}
	
private:
	static std::map<const char*, ID3D11ShaderResourceView*> m_texturePool_c;
	static std::map<const wchar_t*, ID3D11ShaderResourceView*> m_texturePool_w;

	static HRESULT CreateTextureFromFile(_In_ ID3D11Device* d3dDevice,
		_In_z_ const wchar_t* szFileName,
		_Out_opt_ ID3D11ShaderResourceView** textureView,
		_Out_opt_ DirectX::TexMetadata* pTexInfo = nullptr
	);

	static HRESULT CreateTextureFromFile(_In_ ID3D11Device* d3dDevice,
		_In_z_ const char* szFileName,
		_Out_opt_ ID3D11ShaderResourceView** textureView,
		_Out_opt_ DirectX::TexMetadata* pTexInfo = nullptr
	);

};


