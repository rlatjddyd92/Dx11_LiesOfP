#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_CarcassTail_LeapToAttack :
    public CState
{
private:
    typedef enum {
        AN_ROUTE_FIRST = 11,    //Leap
        AN_ROUTE_LAST_TAIL_L = 0,      //Swip L
        AN_ROUTE_LAST_TAIL_R = 1,      //Swip R
        AN_ROUTE_LAST_HAND_L = 7,      //Scratching L
        AN_ROUTE_LAST_HAND_R = 6,      //Scratching R
    }ANIMNUM;

public:
    CState_CarcassTail_LeapToAttack(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_CarcassTail_LeapToAttack() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _float              m_fIdleTime = {};
    _float              m_fIdleDuration = { 0.2f };
   
    _uint               m_iLastAnim = {};

    _bool               m_isDelayed = { false };

    _uint               m_iRouteTrack = {};

    _bool               m_bHeadingSound = { false };
    _bool               m_bSwingSound = { false };

private:
    _bool               End_Check();
    void                Collider_Check(_double CurTrackPos);
    void                Sound_Check(_double CurTrackPos);

public:
    static CState_CarcassTail_LeapToAttack* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END