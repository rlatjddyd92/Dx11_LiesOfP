#include "NvCloth_Manager.h"

#include "GameInstance.h"
//#include "cuda.h"

CNvCloth_Manager::CNvCloth_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CNvCloth_Manager::Initialize()
{
	m_pContextManagerCallback = new CMyContextManagerCallback(m_pDevice, m_pContext);

	//m_pFactory = NvClothCreateFactoryDX11(m_pContextManagerCallback);
	//m_pSolver = m_pFactory->createSolver();

	return S_OK;
}

void CNvCloth_Manager::Update(_float fTimeDelta)
{
}

CNvCloth_Manager* CNvCloth_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNvCloth_Manager* pInstance = new CNvCloth_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CNvCloth_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNvCloth_Manager::Free()
{
	__super::Free();

	Safe_Delete(m_pContextManagerCallback);

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
