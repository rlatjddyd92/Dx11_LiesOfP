#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_SimonManus_HitFatal : public CState
{
private:
    typedef enum{
        AN_FATAL_START = 2, AN_FATAL_LOOP = 1, AN_FATAL_END = 0
    }ANIMNUM;

public:
    CState_SimonManus_HitFatal(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_SimonManus_HitFatal() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iAnimation_Idle = {};
    _uint               m_iIdleCount = {};

    _float              m_fCurrentTime = { 0.f };
    _float              m_fIdleDuration = { 2.6f };

    _bool               m_isPrevChance = { false };
    _bool               m_isActivatedSpecial = { false };

    _float              m_fHitFatalTime = { 0.f };
    _float              m_fHitFatalDuration = { 3.f };

    _bool*              m_pIsEndAnim = { false };
    _bool*              m_pResetRootMove = { nullptr };

    _uint               m_iAnimCnt = {};

    _uint               m_iDirCnt = {};

public:
    static CState_SimonManus_HitFatal* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END