#pragma once
#include "Base.h"

BEGIN(Engine)

class CFsm;

class ENGINE_DLL CState :
    public CBase
{
protected:
    CState(CFsm* pFsm);
    virtual ~CState() = default;

public:
    _uint				    Get_State() { return m_iStateNum; }

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) = 0;
    virtual HRESULT Start_State(void* pArg = nullptr) = 0;
    virtual void    Update(_float fTimeDelta) = 0;
    virtual void    End_State() = 0;

protected:
    CFsm*                   m_pFsm = { nullptr };

    _float                  m_fCurrentTime = { 0.f };
    _uint 			        m_iStateNum;

    class CGameInstance* m_pGameInstance = { nullptr };

public:
    virtual void Free() override;

};

END