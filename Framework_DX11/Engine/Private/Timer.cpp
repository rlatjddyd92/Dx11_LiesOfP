#include "..\Public\Timer.h"

CTimer::CTimer()
	: m_fTimeDelta(0.f)
{
	ZeroMemory(&m_OldTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_CurTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_OriginTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_CpuTick, sizeof(LARGE_INTEGER));
}

HRESULT CTimer::Initialize()
{
	QueryPerformanceCounter(&m_CurTime);		// 1000
	QueryPerformanceCounter(&m_OldTime);		// 1024	
	QueryPerformanceCounter(&m_OriginTime);		// 1030

	QueryPerformanceFrequency(&m_CpuTick);

	return S_OK;
}

void CTimer::Update()
{
	QueryPerformanceCounter(&m_CurTime);	// 1100
	
	if (m_CurTime.QuadPart - m_OriginTime.QuadPart > m_CpuTick.QuadPart)
	{
		QueryPerformanceFrequency(&m_CpuTick);
		m_OriginTime = m_CurTime;
	}
	
	m_fTimeDelta = float(m_CurTime.QuadPart - m_OldTime.QuadPart) / m_CpuTick.QuadPart;

	if (m_isLack)
	{
		m_fLackTime += m_fTimeDelta;
		if (m_fLackTime >= m_fLackDuration)
		{
			m_isLack = false;
		}

		m_fTimeDelta = m_fLackTimeDelta;
	}

	m_OldTime = m_CurTime;
}

void CTimer::Start_Lack(_float fLackTimeDelta, _float fDuration)
{
	if (fLackTimeDelta <= 0.f)
		m_fLackTimeDelta = 0.0001f;
	else
		m_fLackTimeDelta = fLackTimeDelta;

	m_fLackDuration = fDuration;
	m_fLackTime = 0.f;

	m_isLack = true;
}

CTimer * CTimer::Create()
{
	CTimer*		pInstance = new CTimer();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CTimer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CTimer::Free()
{
	__super::Free();
}









