#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
END

BEGIN(Client)

class CMachine_EffectObj : public CGameObject
{
private:
    CMachine_EffectObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CMachine_EffectObj(const CMachine_EffectObj& Prototype);
    virtual ~CMachine_EffectObj() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg = nullptr);
    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();

private:
    class CEffect_Container*        m_pMachineEffect = { nullptr };

public:
    static CMachine_EffectObj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;
};

END