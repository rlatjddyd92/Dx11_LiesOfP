#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)
class CDeco_Collider :
    public CGameObject
{
public:
	typedef struct
	{
		const _float4x4* pParentWorldMatrix = { nullptr };
	}DECO_COLLIDER_DESC;

protected:
	CDeco_Collider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDeco_Collider(const CDeco_Collider& Prototype);
	virtual ~CDeco_Collider() = default;

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

private:
	CCollider* m_pColliderCom = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CDeco_Collider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END