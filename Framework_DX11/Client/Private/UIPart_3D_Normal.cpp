#include "stdafx.h"
#include "..\Public\UIPart_3D_Normal.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CUIPart_3D_Normal::CUIPart_3D_Normal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CUIPart_3D{ pDevice , pContext }
{
}

CUIPart_3D_Normal::CUIPart_3D_Normal(const CUIPart_3D& Prototype)
    :CUIPart_3D{ Prototype }
{
}

HRESULT CUIPart_3D_Normal::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUIPart_3D_Normal::Initialize(void* pArg)
{
    return S_OK;
}

void CUIPart_3D_Normal::Priority_Update(_float fTimeDelta)
{
}

void CUIPart_3D_Normal::Update(_float fTimeDelta)
{
}

void CUIPart_3D_Normal::Late_Update(_float fTimeDelta)
{
}

HRESULT CUIPart_3D_Normal::Render()
{
    return S_OK;
}

CUIPart_3D_Normal* CUIPart_3D_Normal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    return nullptr;
}

CGameObject* CUIPart_3D_Normal::Clone(void* pArg)
{
    return nullptr;
}

void CUIPart_3D_Normal::Free()
{
}
