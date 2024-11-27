#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CSteppingStone :
	public CGameObject
{
protected:
	CSteppingStone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSteppingStone(const CSteppingStone& Prototype);
	virtual ~CSteppingStone() = default;

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

private:
	_bool m_bShadow = false;
	_bool m_bCollision = false;
	_float m_fCollisonTimer = { 0.f };

	static int iStaticIndex;
	_int m_iIndex = { 0 };

private:
	HRESULT Ready_Components(OBJECT_DEFAULT_DESC* pDesc);

private:
	void Change_Player_Pos();
public:
	static CSteppingStone* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END