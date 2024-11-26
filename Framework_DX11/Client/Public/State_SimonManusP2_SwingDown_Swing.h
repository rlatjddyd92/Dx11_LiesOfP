#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_SimonManusP2_SwingDown_Swing :
    public CState
{
private:
    typedef enum {
        AN_ROUTE_FIRST = 30,        //SwingDown
        AN_ROUTE_LAST_L = 29,       //SwingRight
        AN_ROUTE_LAST_R = 28,       //SwingLeft
    }ANIMNUM;

public:
    CState_SimonManusP2_SwingDown_Swing(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_SimonManusP2_SwingDown_Swing() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster*     m_pMonster = { nullptr };

    _uint               m_iCurLastAnim = {};

    _bool               m_isSwing = { false };

    _uint               m_iRouteTrack = {};

private:
    _bool               End_Check();
    void                Collider_Check();

public:
    static CState_SimonManusP2_SwingDown_Swing* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END