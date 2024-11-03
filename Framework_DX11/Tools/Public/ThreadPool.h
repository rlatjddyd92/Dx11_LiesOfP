#pragma once

#include "Tools_Defines.h"
#include "Base.h"
#include "GameInstance.h"
#include <thread>
#include <condition_variable>
#include <mutex>
#include <future>
#include <functional>

BEGIN(Engine)
class CGameObject;
END

BEGIN(Tools)

class CThreadPool : public CBase
{
	DECLARE_SINGLETON(CThreadPool)

private:
	CThreadPool();
	virtual ~CThreadPool() = default;

public:
	HRESULT Initialize();
	void Start_Work();
	void StopAll();

	template <typename F, typename... Args>
	future<typename result_of<F(Args...)>::type> Add_Work(F f, Args... args);

private:
	class CGameInstance* m_pGameInstance = { nullptr };

	_uint				m_iNumThreads;
	_bool				m_isAllStop = { false };

	vector<thread>		m_WorkerThreads;
	queue<function<void()>> m_Functions;
	condition_variable m_ContionVariable;
	mutex m_Mutex;

private:
	void WorkerThread();

public:
	virtual void Free() override;
};

// F : 리턴타입
// typename... : 가변 길이 템플릿으로 임의의 길이의 인자들을 받아올 수 있음
template <typename F, typename... Args>
future<typename result_of<F(Args...)>::type> CThreadPool::Add_Work(F f, Args... args)
{
	// 비동기적으로 실행되는 함수의 리턴값
	using return_type = typename result_of<F(Args...)>::type;
	auto job = make_shared<packaged_task<return_type()>>(bind(f, args...));

	future<return_type> job_result_future = job->get_future();
	{	
		//m_Mutex.lock();
		lock_guard<mutex> lock(m_Mutex);
		m_Functions.push([job]() { (*job)(); });
	}
	m_ContionVariable.notify_one();

	return job_result_future;
}

END

