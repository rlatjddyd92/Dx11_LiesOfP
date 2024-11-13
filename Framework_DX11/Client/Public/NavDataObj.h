#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CNavigation;
END

BEGIN(Client)
class CNavDataObj :
	public CGameObject
{
private:
	CNavDataObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavDataObj(const CNavDataObj& Prototype);
	virtual ~CNavDataObj() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();

public:
	class CNavigation* m_pNavigationCom = { nullptr };

public:
	static CNavDataObj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END