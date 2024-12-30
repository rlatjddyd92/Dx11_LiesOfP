#include "stdafx.h"
#include "..\Public\UIPart_3D_Path.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CUIPart_3D_Path::CUIPart_3D_Path(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CUIPart_3D{ pDevice , pContext }
{
}

CUIPart_3D_Path::CUIPart_3D_Path(const CUIPart_3D& Prototype)
    :CUIPart_3D{ Prototype }
{
}

HRESULT CUIPart_3D_Path::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUIPart_3D_Path::Initialize(void* pArg)
{
    return S_OK;
}

void CUIPart_3D_Path::Priority_Update(_float fTimeDelta)
{
}

void CUIPart_3D_Path::Update(_float fTimeDelta)
{
}

void CUIPart_3D_Path::Late_Update(_float fTimeDelta)
{
}

HRESULT CUIPart_3D_Path::Render()
{
    return S_OK;
}

CUIPart_3D_Path* CUIPart_3D_Path::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    return nullptr;
}

CGameObject* CUIPart_3D_Path::Clone(void* pArg)
{
    return nullptr;
}

void CUIPart_3D_Path::Free()
{
}
