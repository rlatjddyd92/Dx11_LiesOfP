#include "UIPart_3D.h"

CUIPart_3D::CUIPart_3D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject{ pDevice , pContext }
{
}

CUIPart_3D::CUIPart_3D(const CUIPart_3D& Prototype)
	:CGameObject{ Prototype }
{
}

HRESULT CUIPart_3D::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CUIPart_3D::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

void CUIPart_3D::Priority_Update(_float fTimeDelta)
{
}

void CUIPart_3D::Update(_float fTimeDelta)
{
}

void CUIPart_3D::Late_Update(_float fTimeDelta)
{
}

HRESULT CUIPart_3D::Render()
{
	return E_NOTIMPL;
}

CUIPart_3D* CUIPart_3D::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return nullptr;
}

CGameObject* CUIPart_3D::Clone(void* pArg)
{
	return nullptr;
}

void CUIPart_3D::Free()
{
}
