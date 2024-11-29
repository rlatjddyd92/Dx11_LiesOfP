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

class CTreasureBox :
	public CGameObject
{
protected:
	CTreasureBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTreasureBox(const CTreasureBox& Prototype);
	virtual ~CTreasureBox() = default;

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
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CRigidBody* m_pRigidBodyCom = { nullptr };

private:
	_bool m_bShadow = { false };
	_bool m_bCollision = { false };
	_bool m_bOpen = { false };

	_int m_iAnim_Close_Idle = { 0 };
	_int m_iAnim_Open = { 0 };
	_int m_iAnim_Open_Idle = { 0 };

private:
	HRESULT Ready_Components(OBJECT_DEFAULT_DESC* pDesc);

public:
	static CTreasureBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END