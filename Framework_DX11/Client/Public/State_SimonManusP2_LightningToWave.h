#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_SimonManusP2_LightningToWave :
    public CState
{
private:
    typedef enum {
        AN_ROUTE_FIRST = 46,        //SwingDown
        AN_ROUTE_LAST = 45,       //SwingRight
    }ANIMNUM;

public:
    CState_SimonManusP2_LightningToWave(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_SimonManusP2_LightningToWave() = default;

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

    _bool               m_isPrevChance = { false };
    _bool               m_isActivatedSpecial = { false };

    _bool*              m_pResetRootMove = { nullptr };

    _uint               m_iRouteTrack = {};

private:
    _bool               End_Check();

public:
    static CState_SimonManusP2_LightningToWave* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END