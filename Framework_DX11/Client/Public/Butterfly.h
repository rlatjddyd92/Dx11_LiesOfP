#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CButterfly :
    public CGameObject
{
private:
	CButterfly(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CButterfly(const CButterfly& Prototype);
	virtual ~CButterfly() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Offset(_float fX, _float fY, _float fZ) { m_vPosOffset = { fX, fY, fZ }; }
private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	class CEffect_Container* m_Effect = { nullptr };

private:
	_int	m_iAnimIndex = { 0 };
	_bool	m_bFirstPosSetting = { false };

	_Vec3 m_vNewPos = {};
	_Vec3 m_vPosOffset= {};

	_float           m_fEmissiveMask = {};

private:
	HRESULT Ready_Components(OBJECT_DEFAULT_DESC* pDesc);

public:
	static CButterfly* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END