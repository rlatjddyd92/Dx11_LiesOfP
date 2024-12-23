#include "MyContextManagerCallback.h"
#include "GameInstance.h"

CMyContextManagerCallback::CMyContextManagerCallback(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
}

void CMyContextManagerCallback::acquireContext()
{
}

void CMyContextManagerCallback::releaseContext()
{
}

ID3D11Device* CMyContextManagerCallback::getDevice() const
{
	return m_pDevice;
}

ID3D11DeviceContext* CMyContextManagerCallback::getContext() const
{
	return m_pContext;
}

bool CMyContextManagerCallback::synchronizeResources() const
{
	return mSynchronizeResources;
}
