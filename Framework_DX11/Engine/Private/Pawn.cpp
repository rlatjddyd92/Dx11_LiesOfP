#include "Pawn.h"
#include "GameInstance.h"
#include "PartObject.h"

CPawn::CPawn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CPawn::CPawn(const CPawn& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CPawn::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPawn::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPawn::Priority_Update(_float fTimeDelta)
{
}

void CPawn::Update(_float fTimeDelta)
{
}

void CPawn::Late_Update(_float fTimeDelta)
{
}

HRESULT CPawn::Render()
{
	return S_OK;
}

void CPawn::OnCollisionEnter(CGameObject* pOther)
{
}

void CPawn::OnCollisionStay(CGameObject* pOther)
{
}

void CPawn::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CPawn::Add_PartObject(_uint iPartID, const _wstring& strPrototypeTag, void* pArg)
{
	CGameObject* pPartObject = m_pGameInstance->Clone_GameObject(strPrototypeTag, pArg);

	if (nullptr == pPartObject)
		return E_FAIL;

	m_Parts[iPartID] = dynamic_cast<CPartObject*>(pPartObject);

	return S_OK;
}

void CPawn::Free()
{
	__super::Free();

	for (auto& pPartObject : m_Parts)
		Safe_Release(pPartObject);

	m_Parts.clear();
}
