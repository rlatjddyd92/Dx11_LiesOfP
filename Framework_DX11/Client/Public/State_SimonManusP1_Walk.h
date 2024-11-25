#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_SimonManusP1_Walk : public CState
{
private:
    typedef enum {
        AN_WALK_BACK = 49, AN_WALK_FRONT = 50, AN_WALK_LEFT = 51, AN_WALK_RIGHT = 52
        , AN_TURNLEFT = 47, AN_TURNRIGHT = 48
    }ANIMNUM;

public:
    CState_SimonManusP1_Walk(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_SimonManusP1_Walk() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _float          m_fWalkSpeed = { 3.f };
public:
    static CState_SimonManusP1_Walk* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END