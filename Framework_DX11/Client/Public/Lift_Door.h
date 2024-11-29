#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CRigidBody;
END

BEGIN(Client)

class CLift_Door :
    public CGameObject
{
protected:
	CLift_Door(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLift_Door(const CLift_Door& Prototype);
	virtual ~CLift_Door() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Open() { m_bOpen = true; m_bClose = false; }
	void Close() { m_bOpen = false; m_bClose = true; }
private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CRigidBody* m_pRigidBodyCom = { nullptr };

private:
	_bool m_bShadow = false;
	_bool m_bOpen = false;
	_bool m_bClose = false;

	_int m_iAnim_Close = { 0 };
	_int m_iAnim_Close_Idle = { 0 };
	_int m_iAnim_Open = { 0 };
	_int m_iAnim_Open_Idle = { 0 };

private:
	HRESULT Ready_Components();

public:
	static CLift_Door* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END