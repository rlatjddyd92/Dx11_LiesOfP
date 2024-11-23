#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)       //¥Ÿ¿Ã∫˘ »÷µŒ∏£±‚

class CState_SimonManusP2_Route2 :
    public CState
{
private:
    typedef enum {
        AN_ROUTE_FIRST = 54,        //SwingDown
        AN_ROUTE_LAST = 23,       //HighJump
    }ANIMNUM;

public:
    CState_SimonManusP2_Route2(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_SimonManusP2_Route2() = default;

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
    _float              m_fIdleDuration = { 0.2f };

    _Vec4               m_vTargetDir{};
    _Vec3               m_vFlyMoveStack{};

    _bool               m_isPrevChance = { false };
    _bool               m_isActivatedSpecial = { false };
    _bool               m_isJump = { false };
    
    _bool* m_pRootMoveCtr{ nullptr };

    _bool* m_pResetRootMove = { nullptr };

    _uint               m_iRouteTrack = {};

private:
    _bool               End_Check();

public:
    static CState_SimonManusP2_Route2* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END