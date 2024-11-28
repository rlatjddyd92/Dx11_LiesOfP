#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_CarcassTail_LeapAttack :
    public CState
{
private:
    typedef enum {
        AN_ROUTE_FIRST = 10,    //LeapStart
        AN_ROUTE_MIDDLE = 9,    //LeapMiddle
        AN_ROUTE_LAST = 8      //LeapLast
    }ANIMNUM;

public:
    CState_CarcassTail_LeapAttack(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_CarcassTail_LeapAttack() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _float              m_fIdleTime = {};
    _float              m_fIdleDuration = { 0.2f };

    _bool               m_isDelayed = { false };

    _uint               m_iRouteTrack = {};

private:
    _bool               End_Check();
    void                Collider_Check();

public:
    static CState_CarcassTail_LeapAttack* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END