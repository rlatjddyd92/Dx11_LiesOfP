#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_CarcassBigA_Walk : public CState
{
public:
    CState_CarcassBigA_Walk(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_CarcassBigA_Walk() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _float          m_fWalkSpeed = { 2.f };
public:
    static CState_CarcassBigA_Walk* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END