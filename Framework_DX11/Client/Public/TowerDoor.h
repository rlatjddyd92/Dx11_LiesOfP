#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CTowerDoor :
	public CGameObject
{
private:
	CTowerDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTowerDoor(const CTowerDoor& Prototype);

	virtual ~CTowerDoor() = default;
public:
	_bool	Get_IsOpen() { return m_bOpen; }
	void	Set_IsOpen(_bool isOpen) { m_bOpen = isOpen; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CCollider* m_pColliderCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CRigidBody* m_pRigidBodyCom = { nullptr };
	CSound* m_pSoundCom = { nullptr };

private:
	_bool m_bShadow = false;
	_bool m_bOpen = false;
	_bool m_bPlaySound = { false };

	_int m_iAnim_Open = { 0 };

private:
	HRESULT Ready_Components();

public:
	static CTowerDoor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END