#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_SimonManusP2_Die_Talking : public CState
{
public:
    CState_SimonManusP2_Die_Talking(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_SimonManusP2_Die_Talking() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

public:
    static CState_SimonManusP2_Die_Talking* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END