#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
class CRigidBody;
END

BEGIN(Client)

class CLastDoor :
	public CGameObject
{
protected:
	CLastDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLastDoor(const CLastDoor& Prototype);
	virtual ~CLastDoor() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_bool	Get_IsOpen() { return m_bOpen; }
	void	Set_IsOpen(_bool isOpen) { m_bOpen = isOpen; }

private:
	CCollider* m_pColliderCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CRigidBody* m_pRigidBodyCom = { nullptr };

private:
	_bool m_bShadow = false;
	_bool m_bOpen = false;

	_int m_iAnim_Open = { 0 };
private:
	HRESULT Ready_Components();

public:
	static CLastDoor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END