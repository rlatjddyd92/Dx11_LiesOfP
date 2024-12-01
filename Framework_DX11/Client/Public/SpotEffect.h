#pragma once

#include "Client_Defines.h"
#include "EffectObject.h"


BEGIN(Client)

class CSpotEffect final : public CEffectObject
{
private:
	CSpotEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpotEffect(const CGameObject& Prototype);
	virtual ~CSpotEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

protected:
	HRESULT Ready_Components(_wstring strEffectTag);

public:
	static CSpotEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END