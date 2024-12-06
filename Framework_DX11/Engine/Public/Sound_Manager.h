#pragma once
#include "Base.h"

#include "fmod.h"
#include "fmod.hpp"

#include <io.h>

#define SOUND_MAX 1.0f
#define SOUND_MIN 0.0f
#define SOUND_DEFAULT 0.5f
#define SOUND_WEIGHT 0.1f

BEGIN(Engine)

class ENGINE_DLL CSound_Manager final : public CBase
{
private:
	CSound_Manager();
	virtual ~CSound_Manager() = default;

public:
	map<TCHAR*, FMOD::Sound*>& Get_Sounds() { return m_Sounds; }

	FMOD::System* Get_System() { return m_pSystem; }

	void Set_Listener(class CGameObject* pListener) { m_pListenerObject = pListener; }

public:
	HRESULT Initialize();
	void	Update();

public:
	void Play_BGM(const TCHAR* pSoundKey, _float* fVolume);
	void Play_Effect(const TCHAR* pSoundKey, _float fVolume);
	void Stop_BGM();
	void Pause_BGM();
	void SetVolume_BGM(_float fVolume);

public:
	void Play_ENV(const TCHAR* pSoundKey, _float* fVolume);
	void Stop_ENV();
	void Pause_ENV();

public:
	void LoadSoundFile(const char* pFolderName);
	void Clear();

private:
	map<TCHAR*, FMOD::Sound*>::iterator Find_Sound(const TCHAR* pSoundKey);


private:
	// 사운드 리소스 정보를 갖는 객체 
	map<TCHAR*, FMOD::Sound*> m_Sounds;

	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD::Channel* m_pBGMChannel;
	FMOD::Channel* m_pEffectChannel;
	FMOD::Channel* m_pENVChannel;

	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	//FMOD::System* m_pSystem;
	FMOD::System* m_pSystem = { nullptr };

	_float* m_pBGMVolume = { nullptr };
	_float* m_pEnvVolume = { nullptr };

	class CGameObject* m_pListenerObject = { nullptr };
public:
	static CSound_Manager* Create();
	virtual void Free() override;
};

END