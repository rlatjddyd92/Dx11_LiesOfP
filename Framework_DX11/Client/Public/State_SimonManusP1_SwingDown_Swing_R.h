#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_SimonManusP1_SwingDown_Swing_R :
    public CState
{
private:
    typedef enum {
        AN_ROUTE_FIRST = 28,        //SwingDown
        AN_ROUTE_LAST = 27,       //SwingRight
    }ANIMNUM;

public:
    CState_SimonManusP1_SwingDown_Swing_R(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_SimonManusP1_SwingDown_Swing_R() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };
    
    _bool               m_isSwing = { false };

    _uint               m_iRouteTrack = {};

private:
    _bool               End_Check();
    void                Collider_Check();

public:
    static CState_SimonManusP1_SwingDown_Swing_R* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END