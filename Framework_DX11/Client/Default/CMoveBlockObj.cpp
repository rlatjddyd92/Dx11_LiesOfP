#include "stdafx.h"
#include "CMoveBlockObj.h"
#include "GameInstance.h"

#include "BossDoor_Veli.h"

CMoveBlockObj::CMoveBlockObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CMoveBlockObj::CMoveBlockObj(const CMoveBlockObj& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CMoveBlockObj::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMoveBlockObj::Initialize(void* pArg)
{
	MOVEBLOCK_DESC* pDesc = static_cast<MOVEBLOCK_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Effect()))
        return E_FAIL;

	return S_OK;
}

void CMoveBlockObj::Priority_Update(_float fTimeDelta)
{
    m_pVeliEffect->Priority_Update(fTimeDelta);
}

void CMoveBlockObj::Update(_float fTimeDelta)
{
    m_pVeliEffect->Update(fTimeDelta);
}

void CMoveBlockObj::Late_Update(_float fTimeDelta)
{
    m_pVeliEffect->Late_Update(fTimeDelta);
}

HRESULT CMoveBlockObj::Render()
{
    return E_NOTIMPL;
}

void CMoveBlockObj::OnCollisionEnter(CGameObject* pOther)
{
}

void CMoveBlockObj::OnCollisionStay(CGameObject* pOther)
{
}

void CMoveBlockObj::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CMoveBlockObj::Ready_Components(MOVEBLOCK_DESC* pDesc)
{
    return E_NOTIMPL;
}

HRESULT CMoveBlockObj::Ready_Effect()
{
    CBossDoor_Veli::BOSSDOOR_DESC DoorDesc = {};
    DoorDesc.fRotationPerSec = XMConvertToRadians(90.f);
    DoorDesc.fSpeedPerSec = 1.f;
    DoorDesc.iLevelIndex = LEVEL_GAMEPLAY;
    // 자기 걸로 채워야 함.
    DoorDesc.vPos = _Vec3();
    DoorDesc.vRotation = _Vec3();
    DoorDesc.vScale = _Vec3();

    m_pVeliEffect = static_cast<CBossDoor_Veli*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_BossDoor_Veli"), &DoorDesc));

    if (nullptr == m_pVeliEffect)
        return E_FAIL;

    return S_OK;
}

CMoveBlockObj* CMoveBlockObj::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    return nullptr;
}

CGameObject* CMoveBlockObj::Clone(void* pArg)
{
    return nullptr;
}

void CMoveBlockObj::Free()
{


    Safe_Release(m_pVeliEffect);
}
