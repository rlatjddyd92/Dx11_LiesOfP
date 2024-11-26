#include "Sound.h"
#include "State.h"
#include "GameObject.h"

#include "GameInstance.h"

CSound::CSound(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
	m_eComponentType = SOUND;
}

CSound::CSound(const CSound& Prototype)
	:CComponent{ Prototype }
{
	m_eComponentType = SOUND;
}

HRESULT CSound::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSound::Initialize(void* pArg)
{
	m_pSystem = m_pGameInstance->Get_SoundSystem();

	return S_OK;
}

void CSound::Update(_float fTimeDelta)
{
	m_pChannel->isPlaying(&m_isPlaying);
	m_pChannel->getPaused(&m_isPause);

	if (!m_pOwner->IsActive())
		m_pChannel->stop();

	_Vec4 vOwnerPos = m_pOwner->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	FMOD_VECTOR vSoundPos = { vOwnerPos.x, vOwnerPos.y, vOwnerPos.z };
	m_pChannel->set3DAttributes(&vSoundPos, nullptr);

	m_pSystem->update();
}

void CSound::Play3D(const TCHAR* pSoundKey, _float fVolume)
{
	if (m_isPlaying)
		m_pChannel->stop();

	map<TCHAR*, FMOD::Sound*> Sounds = m_pGameInstance->Get_Sounds();

	map<TCHAR*, FMOD::Sound*>::iterator iter;
	iter = find_if(Sounds.begin(), Sounds.end(), [&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == Sounds.end())
		return;


	_Vec4 vOwnerPos = m_pOwner->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	m_vSoundPos = { XMVectorGetX(vOwnerPos), XMVectorGetY(vOwnerPos), XMVectorGetZ(vOwnerPos) };
	m_pChannel->set3DAttributes(&m_vSoundPos, nullptr);

	FMOD_RESULT result;

	result = m_pSystem->playSound(iter->second, 0, FALSE, &m_pChannel);
	if (result != FMOD_OK)
	{
		return;
	}
	m_pChannel->setMode(FMOD_3D | FMOD_LOOP_NORMAL);
	m_pChannel->setVolume(fVolume);
}

void CSound::Play3D_Repeat(const TCHAR* pSoundKey, _float fVolume)
{
	if (!m_isPlaying)
	{
		Play3D(pSoundKey, fVolume);
	}
}

void CSound::Play2D(const TCHAR* pSoundKey, _float fVolume)
{
	if (m_isPlaying)
		m_pChannel->stop();

	map<TCHAR*, FMOD::Sound*> Sounds = m_pGameInstance->Get_Sounds();

	map<TCHAR*, FMOD::Sound*>::iterator iter;
	iter = find_if(Sounds.begin(), Sounds.end(), [&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == Sounds.end())
		return;

	//_Vec4 vOwnerPos = m_pOwner->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	//FMOD_VECTOR vSoundPos = { vOwnerPos.x,vOwnerPos.y,vOwnerPos.z };
	//m_pChannel->set2DAttributes(&vSoundPos, nullptr);

	FMOD_RESULT result;

	result = m_pSystem->playSound(iter->second, 0, FALSE, &m_pChannel);
	if (result != FMOD_OK)
	{
		return;
	}
	m_pChannel->setMode(FMOD_2D);
	m_pChannel->setVolume(fVolume);
}

void CSound::Play2D_Repeat(const TCHAR* pSoundKey, _float fVolume)
{
	if (m_isPlaying)
	{
		Play2D(pSoundKey, fVolume);
	}
}

void CSound::Pause()
{
	m_pChannel->setPaused(!m_isPause);
}

void CSound::Stop()
{
	m_pChannel->stop();
	m_pSystem->update();
}

void CSound::Mute()
{
	m_isMute = true;
	m_pChannel->setMute(m_isMute);
	m_pSystem->update();
}

void CSound::UnMute()
{
	m_isMute = false;
	m_pChannel->setMute(m_isMute);
}

void CSound::Set_Volume(_float fVolume)
{
	m_pChannel->setVolume(fVolume);
}

void CSound::Set_PlaySpeed(_float fSpeedRatio)
{
	_float fCurrentFrequency = {};
	m_pChannel->getFrequency(&fCurrentFrequency);
	
	//44100.f
	m_pChannel->setFrequency(fCurrentFrequency * fSpeedRatio);
}

CSound* CSound::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSound* pInstance = new CSound(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSound"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CComponent* CSound::Clone(void* pArg)
{
	CSound* pInstance = new CSound(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CSound"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSound::Free()
{
	__super::Free();

	m_pChannel->stop();
}
