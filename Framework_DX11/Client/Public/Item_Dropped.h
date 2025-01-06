#pragma once

#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Engine)
class CCollider;
END
BEGIN(Client)

class CItem_Dropped :
    public CGameObject
{
public:
	typedef struct
	{
		_Vec4		vParentPos = {};
		_int		iItemIndex = { -1 };
	}ITEM_DROPPED_DESC;
private:
	CItem_Dropped(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem_Dropped(const CItem_Dropped& Prototype);
	virtual ~CItem_Dropped() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnCollisionEnter(CGameObject* pOther) override;
	virtual void OnCollisionStay(CGameObject* pOther) override;
	virtual void OnCollisionExit(CGameObject* pOther) override;

public:
	_int	Get_ItemIndex() { return m_iItemIndex; };

private:
	CCollider* m_pColliderCom = { nullptr };
	class CEffect_Container* m_Effect = { nullptr };

private:
	_int	m_iItemIndex = { -1 };

private:
	HRESULT Ready_Components();

public:
	static CItem_Dropped* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END