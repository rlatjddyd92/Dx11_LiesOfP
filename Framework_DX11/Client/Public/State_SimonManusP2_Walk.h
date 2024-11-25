#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_SimonManusP2_Walk : public CState
{
private:
    typedef enum {
        AN_WALK_BACK = 58, AN_WALK_FRONT = 59, AN_WALK_LEFT = 60, AN_WALK_RIGHT = 61
    }ANIMNUM;

public:
    CState_SimonManusP2_Walk(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_SimonManusP2_Walk() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _float          m_fWalkSpeed = { 3.f };
public:
    static CState_SimonManusP2_Walk* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END