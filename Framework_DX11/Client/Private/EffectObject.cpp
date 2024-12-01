#include "stdafx.h"
#include "EffectObject.h"

#include "GameInstance.h"

CEffectObject::CEffectObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CEffectObject::CEffectObject(const CGameObject& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CEffectObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEffectObject::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
    {
        return S_OK;
    }
}

void CEffectObject::Priority_Update(_float fTimeDelta)
{
}

void CEffectObject::Update(_float fTimeDelta)
{
}

void CEffectObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CEffectObject::Render()
{
    return S_OK;
}

HRESULT CEffectObject::Render_LightDepth()
{
    return S_OK;
}

void CEffectObject::Play_Sound(WEP_SOUND_TYPE eType, const TCHAR* pSoundKey, _uint iHandIndex)
{
	//_float fVolume = {};
	//switch (eType)
	//{
	//case SOUND_WEAPON:
	//	fVolume = g_fVoiceVolume;
	//	break;
	//case SOUND_EFFECT1:
	//case SOUND_EFFECT2:
	//	fVolume = ;
	//	break;
	//default:
	//	return;
	//}
	m_pSoundCom[eType]->Play2D(pSoundKey, g_fEffectVolume);
}

HRESULT CEffectObject::Ready_Components()
{
	/* FOR.Com_WeaponSound */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
		TEXT("Com_WeaponSound"), reinterpret_cast<CComponent**>(&m_pSoundCom[WEP_SOUND_WEAPON]))))
		return E_FAIL;
	m_pSoundCom[WEP_SOUND_WEAPON]->Set_Owner(this);

	/* FOR.Com_EffectSound */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
		TEXT("Com_EffectSound"), reinterpret_cast<CComponent**>(&m_pSoundCom[WEP_SOUND_EFFECT1]))))
		return E_FAIL;
	m_pSoundCom[WEP_SOUND_EFFECT1]->Set_Owner(this);

	/* FOR.Com_EffectSound2 */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
		TEXT("Com_EffectSound2"), reinterpret_cast<CComponent**>(&m_pSoundCom[WEP_SOUND_EFFECT2]))))
		return E_FAIL;
	m_pSoundCom[WEP_SOUND_EFFECT2]->Set_Owner(this);

	return S_OK;
    return S_OK;
}

void CEffectObject::Free()
{
    __super::Free();
}
