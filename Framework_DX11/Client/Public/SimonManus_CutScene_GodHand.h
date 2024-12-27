#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CSimonManus_CutScene_GodHand final : public CGameObject
{
private:
	CSimonManus_CutScene_GodHand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSimonManus_CutScene_GodHand(const CSimonManus_CutScene_GodHand& Prototype);
	virtual ~CSimonManus_CutScene_GodHand() = default;

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

	class CEffect_Container* m_pParticleEffect = { nullptr };

	_float           m_fEmissiveMask = {};

	_Vec4            m_vRimLightColor = {};

	_bool			 m_bAnimSpeedUp = { false };
private:

	HRESULT Ready_Components();


public:
	static CSimonManus_CutScene_GodHand* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END