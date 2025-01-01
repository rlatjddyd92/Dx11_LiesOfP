
#include "stdafx.h"
#include "..\Public\UIRender_3D.h"

#include "GameInstance.h"

CUIRender_3D::CUIRender_3D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject{ pDevice ,pContext }
{
}

CUIRender_3D::CUIRender_3D(const CUIRender_3D& Prototype)
	:CGameObject{ Prototype }
{
}

HRESULT CUIRender_3D::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CUIRender_3D::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

void CUIRender_3D::Priority_Update(_float fTimeDelta)
{
}

void CUIRender_3D::Update(_float fTimeDelta)
{
}

void CUIRender_3D::Late_Update(_float fTimeDelta)
{
}

HRESULT CUIRender_3D::Render()
{
	return E_NOTIMPL;
}

HRESULT CUIRender_3D::Ready_Components()
{
	return E_NOTIMPL;
}

CUIRender_3D* CUIRender_3D::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return nullptr;
}

CGameObject* CUIRender_3D::Clone(void* pArg)
{
	return nullptr;
}

void CUIRender_3D::Free()
{
}
