#pragma once
#include "Base.h"


BEGIN(Engine)

class CCollider_Manager :
	public CBase
{
private:
	CCollider_Manager();
	virtual ~CCollider_Manager() = default;

public:
	HRESULT Initialize();
	void	Update();

public:
	HRESULT Add_ColliderList(class CCollider* pCollider);

private:
	class CGameInstance* m_pGameInstance = { nullptr };

	list<class CCollider*>  m_ColliderList;

public:
	static CCollider_Manager* Create();
	virtual void Free() override;
};

END