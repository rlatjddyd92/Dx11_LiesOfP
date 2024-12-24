#pragma once
#include "Base.h"

#include "MyContextManagerCallback.h"

BEGIN(Engine)

class CTransform;
class CGameObject;

class ENGINE_DLL CNvCloth_Manager :
    public CBase
{

private:
    CNvCloth_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CNvCloth_Manager() = default;

public:

public:
    HRESULT Initialize();
    void Update(_float fTimeDelta);

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };

private:
    CMyContextManagerCallback* m_pContextManagerCallback = { nullptr };

    Factory*    m_pFactory = { nullptr };
    Solver*     m_pSolver = { nullptr };

public:
    static CNvCloth_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free() override;
};

END