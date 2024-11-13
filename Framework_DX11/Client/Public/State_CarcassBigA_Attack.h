#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_CarcassBigA_Attack : public CState
{
public:
    CState_CarcassBigA_Attack(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_CarcassBigA_Attack() = default;

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

    //
    _bool*              m_pIsEndAnim = { false };
    _bool*              m_pColliderCtrs = { nullptr };
    _vector             m_vAttackDir = {};

    _int              m_iAtkAnimNum = {};
    _int              m_iAttackCnt = {};


public:
    static CState_CarcassBigA_Attack* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END