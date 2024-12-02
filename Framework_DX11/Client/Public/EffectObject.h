#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Effect_Container.h"


#include "Sound.h"

BEGIN(Client)

class CEffectObject abstract : public CGameObject
{
public:
	typedef struct
	{
		_Vec3 vPos{};
		_Vec3 vDir{};
		_float fLifeDuration{};
		_wstring strEffectTag = TEXT("");
	}EFFECTOBJ_DESC;

public:
	enum WEP_SOUND_TYPE { WEP_SOUND_WEAPON, WEP_SOUND_EFFECT1, WEP_SOUND_EFFECT2, WEP_SOUND_END };

protected:
	CEffectObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffectObject(const CGameObject& Prototype);
	virtual ~CEffectObject() = default;

public:
	class CSound* Get_SoundCom(WEP_SOUND_TYPE eType) { return m_pSoundCom[eType]; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;

public:
	virtual void Play_Sound(WEP_SOUND_TYPE eType, const TCHAR* pSoundKey, _uint iHandIndex = 1);

protected:
	class CSound* m_pSoundCom[WEP_SOUND_END] = { nullptr, };
	class CEffect_Container* m_pEffect = { nullptr };

protected:
	_float			m_fLifeTime = {};
	_float			m_fLifeDuration = {};

protected:
	HRESULT Ready_Components();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

};

END