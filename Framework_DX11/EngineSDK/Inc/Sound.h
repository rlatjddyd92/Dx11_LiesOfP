#pragma once
#include "Component.h"

#include "fmod.h"
#include "fmod.hpp"

BEGIN(Engine)

class ENGINE_DLL CSound :
	public CComponent
{

private:
	CSound(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSound(const CSound& Prototype);
	virtual ~CSound() = default;

public:
	void Set_Owner(class CGameObject* pOwner) { m_pOwner = pOwner; }

	_bool	Get_IsPlaying() { return m_isPlaying; }
	_bool	Get_IsPause() { return m_isPause; }
	_bool	Get_IsMute() { return m_isMute; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(_float fTimeDelta);

public:
	void	Play3D(const TCHAR* pSoundKey, _float fVolume);
	void	Play3D_Repeat(const TCHAR* pSoundKey, _float fVolume);

	void	Play2D(const TCHAR* pSoundKey, _float fVolume);
	void	Play2D_Repeat(const TCHAR* pSoundKey, _float fVolume);

	void	Pause();
	void	Stop();

	void	Mute();
	void	UnMute();

	void	Set_Volume(_float fVolume);
	void	Set_PlaySpeed(_float fSpeedRatio);

private:
	class CGameObject* m_pOwner = { nullptr };

	FMOD::Channel* m_pChannel;

	FMOD::System* m_pSystem = { nullptr };

	_bool m_isPlaying = { false };
	_bool m_isPause = { false };
	_bool m_isMute = { false };

	FMOD_VECTOR		m_vSoundPos;

public:
	static CSound* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END
