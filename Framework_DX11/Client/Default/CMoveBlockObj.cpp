#include "stdafx.h"
#include "CMoveBlockObj.h"
#include "GameInstance.h"

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

	return S_OK;
}

void CMoveBlockObj::Priority_Update(_float fTimeDelta)
{
}

void CMoveBlockObj::Update(_float fTimeDelta)
{
}

void CMoveBlockObj::Late_Update(_float fTimeDelta)
{
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
}
