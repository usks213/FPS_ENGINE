// CSoundクラス定義 (Sound.h)
//
#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <assert.h>

#include <objbase.h>
#include <vector>
#include <mmreg.h>
#include <unordered_map>
#include <map>
#include <string>

#include <comip.h>
#include <comdef.h>

#include <xaudio2.h>

#include <initguid.h>
#include <mfidl.h>
#include <mfapi.h>
#include <mfreadwrite.h>

//-------- マクロ
#ifdef COMPTR
#undef COMPTR
#endif
#define COMPTR(type) _com_ptr_t<_com_IIID<type,&__uuidof(type)>>
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x)			if(x){x->Release();x=nullptr;}
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x)	if(x){delete[]x;x=nullptr;}
#endif
#ifndef SAFE_DESTROYVOICE
#define SAFE_DESTROYVOICE(x)	if(x){x->DestroyVoice();x=nullptr;}
#endif
#define MAX_BUFFER_COUNT		3	// ストリーム再生用バッファ数
#define MAX_DUP					16	// 最大多重再生数

enum SoundStatus {
	SS_STOP = 0,
	SS_PLAY,
	SS_PAUSE,

	MAX_SOUNDSTATUS
};

// BGMクラス
struct CSoundStream
{
private:
	IXAudio2SourceVoice*	m_pSourceVoice;
	COMPTR(IMFSourceReader)	m_reader;
	DWORD					m_currentStreamBuffer;
	std::vector<BYTE>		m_buffers[MAX_BUFFER_COUNT];
	bool					m_endOfStream;
	SoundStatus				m_status;

public:
	CSoundStream();
	virtual ~CSoundStream();
	HRESULT Open(IXAudio2* pXAudio2, LPCWSTR pszPath);
	void Update();
	void Close();
	void Play();
	void Stop();
	void Pause();
	void Resume();
	bool IsPlaying();
	void SetVolume(float fVol);
	float GetVolume();
};

// SEクラス
struct CSoundEffect
{
public:
	IXAudio2SourceVoice*	m_pSourceVoice[MAX_DUP];
	SoundStatus				m_status[MAX_DUP];
	std::vector<BYTE>		m_buffer;
	DWORD					m_bufferLength;

public:
	CSoundEffect();
	virtual ~CSoundEffect();
	HRESULT Open(IXAudio2* pXAudio2, LPCWSTR pszPath);
	void Close();
	void Play();
	void Stop(int n = -1);
	void Pause();
	void Resume();
	bool IsPlaying();
	void SetVolume(float fVol);
	float GetVolume();
};

// サウンド マネージャ クラス
class CSound
{
private:
	static IXAudio2*				m_pXAudio2;
	static IXAudio2MasteringVoice*	m_pMasteringVoice;
	static std::map<const std::wstring, CSoundStream*, std::less<>>			m_BgmPool;
	static std::map<const std::wstring, CSoundEffect*, std::less<>>			m_SePool;

	static void LoadBGM(std::string filename);
	static void LoadSE(std::string filename);


public:
	CSound(void);
	virtual ~CSound(void);

	static void Init(void);
	static void Update(void);
	static void Fin(void);
	static void PlayBGM(std::string  filename, float fVol = 1.0f);
	static void PlaySE(std::string  filename, float fVol = 1.0f);
	static bool IsPlaying(std::string  filename);
	static void StopBGM(std::string  filename);
	static void StopSE(std::string  filename);
	static void Pause();
	static void Resume();
	static void SetVolume(float fVol);
	static float GetVolume();
	static void SetVolumeBGM(std::string  filename, float fVol);
	static float GetVolumeBGM(std::string  filename);
	static void SetVolumeSE(std::string  filename, float fVol);
	static float GetVolumeSE(std::string  filename);

	static CSoundStream* GetBGM(std::string filename);
	static CSoundEffect* GetSE(std::string filename);
};