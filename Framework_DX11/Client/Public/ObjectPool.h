#pragma once
#include "Client_Defines.h"

#include "GameObject.h"

BEGIN(Client)

template <typename T>
class CObjectPool final
{
public:

	// Get하면 큐에서 빠져나옴
	static T* Get_GameObject()
	{
		if (Objects.empty())
			return nullptr;

		T* pObj = Objects.front();
		if (pObj == nullptr)
			return nullptr;

		Objects.pop();

		return pObj;
	}

	// 다 쓰고나면 무조건 반환해주기
	static _bool Return_GameObject(T* pObj)
	{
		if (pObj == nullptr)
			return nullptr;
		Objects.push(pObj);

		return TRUE;
	}

	static void Clear()
	{
		while (!Objects.empty())   Objects.pop();

	}

	// 레이어에 추가해두면 따로 호출하지 않아도 됨
	static void Free()
	{
		while (!Objects.empty())
		{
			T* pObj = Objects.front();
			Safe_Release(pObj);
			Objects.pop();
		}
	}

	static queue<T*>& Get_Queue() { return Objects; }

private:
	static queue<T*> Objects;


private:
	CObjectPool() {}
	~CObjectPool() = default;

};

template <typename T>
queue<T*> CObjectPool<T>::Objects;

END