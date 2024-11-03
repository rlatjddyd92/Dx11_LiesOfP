#include "Fsm.h"
#include "State.h"
#include "GameObject.h"

CFsm::CFsm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
	m_eComponentType = FSM;
}

CFsm::CFsm(const CFsm& Prototype)
	:CComponent{ Prototype }
{
	m_eComponentType = FSM;
}

HRESULT CFsm::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFsm::Initialize(void* pArg)
{
	m_pOwner = static_cast<CGameObject*>(pArg);

	return S_OK;
}

void CFsm::Update(_float fTimeDelta)
{
	m_pNowState->Update(fTimeDelta);
}

void CFsm::Release_States()
{
	for (auto& iter : m_States)
		Safe_Release(iter);

	Safe_Release(m_pNowState);
	Safe_Release(m_pPreState);

	m_States.clear();
}

void CFsm::Add_State(CState* pState)
{
	m_States.push_back(pState);
}

void CFsm::Change_State(_uint iState, void* pArg)
{
	Safe_Release(m_pPreState);
	Safe_Release(m_pNowState);

	m_pPreState = m_pNowState;
	Safe_AddRef(m_pPreState);

	m_pNowState->End_State();

	for (int i = 0; i < m_States.size(); ++i)
	{
		if (iState == m_States[i]->Get_State())
		{
			m_pNowState = m_States[i];
			Safe_AddRef(m_pNowState);

			m_pNowState->Start_State();
			return;
		}
	}
}

void CFsm::Set_State(_uint iState, void* pArg)
{
	for (int i = 0; i < m_States.size(); ++i)
	{
		if (iState == m_States[i]->Get_State())
		{
			m_pNowState = m_States[i];
			m_pPreState = m_States[i];

			Safe_AddRef(m_pNowState);
			Safe_AddRef(m_pPreState);

			m_pNowState->Start_State();
			return;
		}
	}
}

CFsm* CFsm::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFsm* pInstance = new CFsm(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CFsm"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CComponent* CFsm::Clone(void* pArg)
{
	CFsm* pInstance = new CFsm(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CFsm"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFsm::Free()
{
	__super::Free();
}
