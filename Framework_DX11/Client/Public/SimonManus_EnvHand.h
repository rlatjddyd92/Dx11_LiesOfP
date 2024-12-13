#pragma once

#include "Client_Defines.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
class CFSM;
END

BEGIN(Client)

class CSimonManus_EnvHand final : public CGameObject
{
private:
	CSimonManus_EnvHand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSimonManus_EnvHand(const CSimonManus_EnvHand& Prototype);
	virtual ~CSimonManus_EnvHand() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };

	_float           m_fEmissiveMask = {};

	_Vec4            m_vRimLightColor = {};

private:

	HRESULT Ready_Components();


public:
	static CSimonManus_EnvHand* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END