#include "Sound_Manager.h"
#include "Timer_Manager.h"

#include "GameObject.h"

CSound_Manager::CSound_Manager()
	:CBase{}
{
}

HRESULT CSound_Manager::Initialize()
{
	// 사운드를 담당하는 대표객체를 생성하는 함수
	FMOD::System_Create(&m_pSystem);

	m_pSystem->init(300, FMOD_INIT_NORMAL, nullptr);

	if (m_pSystem == nullptr)
		return E_FAIL;

	return S_OK;
}

void CSound_Manager::Update()
{
	if (nullptr != m_pListenerObject)
	{
		_matrix WorldMatrix = m_pListenerObject->Get_Transform()->Get_WorldMatrix();

		_vector vUp = XMVector3Normalize(WorldMatrix.r[1]);
		_vector vLook = XMVector3Normalize(WorldMatrix.r[2]);
		_vector vPos = WorldMatrix.r[3];

		FMOD_VECTOR ListenerPos = { XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos) };
		FMOD_VECTOR ListenerUp = { XMVectorGetX(vUp), XMVectorGetY(vUp), XMVectorGetZ(vUp) };
		FMOD_VECTOR ListenerLook = { XMVectorGetX(vLook), XMVectorGetY(vLook), XMVectorGetZ(vLook) };

		m_pSystem->set3DListenerAttributes(0, &ListenerPos, 0, 0, 0);
	}

	if(nullptr != m_pBGMVolume)
		m_pBGMChannel->setVolume(*m_pBGMVolume);

	if(nullptr != m_pEnvVolume)
		m_pENVChannel->setVolume(*m_pEnvVolume);

	m_pSystem->update();
}

void CSound_Manager::Play_BGM(const TCHAR* pSoundKey, _float* fVolume)
{
	auto iter = Find_Sound(pSoundKey);

	if (iter == m_Sounds.end())
		return;

	_bool isPlaying;
	m_pBGMChannel->isPlaying(&isPlaying);
	if (isPlaying)
		m_pBGMChannel->stop();

	//FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, iter->second, FALSE, &m_pChannelArr[SOUND_BGM]);
	m_pSystem->playSound(iter->second, 0, false, &m_pBGMChannel);

	//FMOD_Channel_SetMode(m_pChannelArr[SOUND_BGM], FMOD_LOOP_NORMAL);
	m_pBGMChannel->setMode(FMOD_LOOP_NORMAL | FMOD_2D);
	//FMOD_Channel_SetVolume(m_pChannelArr[SOUND_BGM], fVolume);
	m_pBGMChannel->setVolume(*fVolume);

	m_pBGMVolume = fVolume;
}

void CSound_Manager::Play_Effect(const TCHAR* pSoundKey, _float* fVolume)
{
	auto iter = Find_Sound(pSoundKey);

	if (iter == m_Sounds.end())
		return;

	m_pSystem->playSound(iter->second, 0, false, &m_pEffectChannel);
	m_pEffectChannel->setMode(FMOD_2D);
	m_pEffectChannel->setVolume(*fVolume);

}

void CSound_Manager::Stop_BGM()
{
	m_pBGMChannel->stop();
}

void CSound_Manager::Pause_BGM()
{
	_bool isPause;
	m_pBGMChannel->getPaused(&isPause);
	m_pBGMChannel->setPaused(!isPause);
}

void CSound_Manager::SetVolume_BGM(_float fVolume)
{
	m_pBGMChannel->setVolume(fVolume);
}

_bool CSound_Manager::IsPlaying_BGM()
{
	_bool isPlaying;
	m_pBGMChannel->isPlaying(&isPlaying);

	return isPlaying;
}

_bool CSound_Manager::IsPause_BGM()
{
	_bool isPause;
	m_pBGMChannel->getPaused(&isPause);
	return isPause;
}

void CSound_Manager::Play_ENV(const TCHAR* pSoundKey, _float* fVolume)
{
	auto iter = Find_Sound(pSoundKey);

	if (iter == m_Sounds.end())
		return;

	_bool isPlaying;
	m_pENVChannel->isPlaying(&isPlaying);
	if (isPlaying)
		m_pENVChannel->stop();

	m_pSystem->playSound(iter->second, 0, false, &m_pENVChannel);

	m_pENVChannel->setMode(FMOD_LOOP_NORMAL | FMOD_2D);
	m_pENVChannel->setVolume(*fVolume);

	m_pEnvVolume = fVolume;
}

void CSound_Manager::Stop_ENV()
{
	m_pENVChannel->stop();
}

void CSound_Manager::Pause_ENV()
{
	_bool isPause;
	m_pBGMChannel->getPaused(&isPause);
	m_pBGMChannel->setPaused(!isPause);
}

void CSound_Manager::Play_Cinematic(const TCHAR* pSoundKey, _float* fVolume)
{
	auto iter = Find_Sound(pSoundKey);

	if (iter == m_Sounds.end())
		return;

	_bool isPlaying;
	m_pCinemticChannel->isPlaying(&isPlaying);
	if (isPlaying)
		m_pCinemticChannel->stop();

	m_pSystem->playSound(iter->second, 0, false, &m_pCinemticChannel);

	m_pCinemticChannel->setMode(FMOD_2D);
	m_pCinemticChannel->setVolume(*fVolume);

	m_pEnvVolume = fVolume;
}

void CSound_Manager::Stop_Cinematic()
{
	m_pCinemticChannel->stop();
}

void CSound_Manager::Pause_Cinematic()
{
	_bool isPause;
	m_pCinemticChannel->getPaused(&isPause);
	m_pCinemticChannel->setPaused(!isPause);
}

void CSound_Manager::LoadSoundFile(const char* pFolderName)
{
	// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
	_finddata_t fd;

	char path[MAX_PATH] = "../../Client/Bin/Resources/Sound/";
	strcat_s(path, MAX_PATH, pFolderName);
	strcat_s(path, MAX_PATH, "/*");

	// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
	intptr_t handle = _findfirst(path, &fd);

	if (handle == -1)
		return;

	int iResult = 0;

	char szCurPath[MAX_PATH] = "../../Client/Bin/Resources/Sound/";
	char szFullPath[MAX_PATH] = "";

	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);
		strcat_s(szFullPath, pFolderName);
		strcat_s(szFullPath, MAX_PATH, "/");
		strcat_s(szFullPath, fd.name);

		FMOD::Sound* pSound = nullptr;

		//FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_HARDWARE, 0, &pSound);
		FMOD_RESULT eRes = m_pSystem->createSound(szFullPath, FMOD_LOOP_OFF | FMOD_3D, 0, &pSound);

		if (eRes == FMOD_OK)
		{
			_int iLength = _int(strlen(fd.name) + 1);

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);

			// 아스키 코드 문자열을 유니코드 문자열로 변환시켜주는 함수
			MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pSoundKey, iLength);
			
			pSound->set3DMinMaxDistance(8.f, 30.f);
			m_Sounds.emplace(pSoundKey, pSound);
		}
		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
		iResult = _findnext(handle, &fd);
	}

	_findclose(handle);
}

void CSound_Manager::Clear()
{
	for (auto& Pair : m_Sounds)
	{
		delete[] Pair.first;

		//FMOD_Sound_Release(Mypair.second);
		Pair.second->release();
	}
	m_Sounds.clear();
}

map<TCHAR*, FMOD::Sound*>::iterator CSound_Manager::Find_Sound(const TCHAR* pSoundKey)
{
	map<TCHAR*, FMOD::Sound*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_Sounds.begin(), m_Sounds.end(), [&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	return iter;
}

CSound_Manager* CSound_Manager::Create()
{
	CSound_Manager* pInstance = new CSound_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CSound_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSound_Manager::Free()
{
	for (auto& Pair : m_Sounds)
	{
		delete[] Pair.first;

		//FMOD_Sound_Release(Mypair.second);
		Pair.second->release();
	}
	m_Sounds.clear();

	m_pSystem->close();
	m_pSystem->release();	//터지는 문제
}
