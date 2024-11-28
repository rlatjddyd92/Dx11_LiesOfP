#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_CarcassTail_ScratchingToSwip :
    public CState
{
private:
    typedef enum {
        AN_ROUTE_FIRST_L = 7,    //Scratching
        AN_ROUTE_FIRST_R = 6,
        AN_ROUTE_LAST_L = 0,      //Swip
        AN_ROUTE_LAST_R = 1
    }ANIMNUM;

public:
    CState_CarcassTail_ScratchingToSwip(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_CarcassTail_ScratchingToSwip() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _float              m_fIdleTime = {};
    _float              m_fIdleDuration = { 0.2f };

    _uint               m_iFirstAnim = {};
    _uint               m_iLastAnim = {};

    _bool               m_isDelayed = { false };

    _uint               m_iRouteTrack = {};

private:
    _bool               End_Check();
    void                Collider_Check();

public:
    static CState_CarcassTail_ScratchingToSwip* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END