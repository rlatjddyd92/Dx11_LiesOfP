#include "stdafx.h"
#include "ThreadPool.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CThreadPool)

CThreadPool::CThreadPool()
	:m_pGameInstance{ CGameInstance::Get_Instance() }
{
}

HRESULT CThreadPool::Initialize()
{
	m_iNumThreads = 4;

	return S_OK;
}

void CThreadPool::Start_Work()
{
    if (m_WorkerThreads.size() >= m_iNumThreads)
        return;

    for (size_t i = 0; i < m_iNumThreads; ++i) 
    {
        m_WorkerThreads.emplace_back([this]() { this->WorkerThread(); });
    }
}

void CThreadPool::StopAll()
{
    m_isAllStop = true;
    m_ContionVariable.notify_all();

    for (auto& work : m_WorkerThreads) {
        work.join();
    }
}

void CThreadPool::WorkerThread()
{
    while (true) {
        unique_lock<mutex> lock(m_Mutex);
        m_ContionVariable.wait(lock, [this]() { return !this->m_Functions.empty() || m_isAllStop; });
        if (m_isAllStop && this->m_Functions.empty()) 
        {
            return;
        }

        // 맨 앞의 job 을 뺀다.
        function<void()> Function = move(m_Functions.front());
        m_Functions.pop();
        lock.unlock();

        // 해당 job 을 수행한다 :)
        Function();
    }
}

void CThreadPool::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
