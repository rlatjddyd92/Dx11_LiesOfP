#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Training_HitFatal : public CState
{
public:
    CState_Training_HitFatal(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_Training_HitFatal() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iAnimation_HitFatal[3] = {};

    _bool*              m_pFatalAttacked = { nullptr };

public:
    static CState_Training_HitFatal* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END