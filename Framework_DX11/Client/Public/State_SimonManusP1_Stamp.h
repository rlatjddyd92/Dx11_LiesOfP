#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

//수정 필요
class CState_SimonManusP1_Stamp : public CState
{
private:
    typedef enum {
        AN_STAMP = 23, AN_SPINBLAST = 25
    }ANIMNUM;

public:
    CState_SimonManusP1_Stamp(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_SimonManusP1_Stamp() = default;

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

    _bool*              m_pIsEndAnim = { false };
    _bool*              m_pResetRootMove = { nullptr };

    _bool               m_bStamp = { false };

private:
    _bool               End_Check();

public:
    static CState_SimonManusP1_Stamp* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END