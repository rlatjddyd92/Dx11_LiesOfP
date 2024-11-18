#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_CarcassBigA_AttackRoute_0 :
    public CState
{
private:
    typedef enum {
        AN_ROUTE_FIRST = 4,    //SwingDwon_Upper_Move
        AN_ROUTE_LAST = 1      //SwingRight
    }ANIMNUM;

private:
    typedef enum {
        DIR_FRONT, DIR_BEHIND
    }DIR;

public:
    CState_CarcassBigA_AttackRoute_0(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_CarcassBigA_AttackRoute_0() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iAnimation_Idle = {};
    _uint               m_iIdleCount = {};
       
    _float              m_fIdleTime = {};
    _float              m_fIdleDuration = { 2.6f };

    _bool               m_isPrevChance = { false };
    _bool               m_isActivatedSpecial = { false };

    _bool*              m_pIsEndAnim = { false };
    _bool*              m_pResetRootMove = { nullptr };

    _uint               m_iRouteTrack = {};

private:
    _bool               End_Check();

public:
    static CState_CarcassBigA_AttackRoute_0* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END