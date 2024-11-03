#include "State.h"

#include "GameInstance.h"
#include "GameObject.h"


CState::CState(CFsm* pFsm)
	:m_pGameInstance{ CGameInstance::Get_Instance() }
	, m_pFsm{ pFsm }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pFsm);
}

void CState::Free()
{
	__super::Free();

	Safe_Release(m_pFsm);
	Safe_Release(m_pGameInstance);
}
