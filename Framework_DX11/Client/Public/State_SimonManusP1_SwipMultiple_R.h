#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_SimonManusP1_SwipMultiple_R :
    public CState
{
private:
    typedef enum {
        AN_ROUTE_FIRST = 18,        //SwingDown
        AN_ROUTE_LAST = 15,       //SwingRight
    }ANIMNUM;

public:
    CState_SimonManusP1_SwipMultiple_R(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_SimonManusP1_SwipMultiple_R() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iRouteTrack = {};

    _bool               m_bSwing = { false };

    _bool               m_bResetCheck = { false };
    _bool               m_bSwing_Sound = { false };
    _bool               m_bStamp_Sound = { false };

private:
    _bool               End_Check();
    void                Collider_Check(_double CurTrackPos);
    void                Effect_Check(_double CurTrackPos);
    void                Control_Sound(_double CurTrackPos);

public:
    static CState_SimonManusP1_SwipMultiple_R* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END