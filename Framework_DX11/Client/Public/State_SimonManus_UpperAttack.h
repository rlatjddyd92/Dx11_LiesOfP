#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_SimonManus_UpperAttack : public CState
{
private:
    typedef enum {
        AN_UPPERATTACK = 16
    }ANIMNUM;
public:
    CState_SimonManus_UpperAttack(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_SimonManus_UpperAttack() = default;

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
    _bool*              m_pColliderCtrs = { nullptr };
    _Vec3               m_vAttackDir = {};



public:
    static CState_SimonManus_UpperAttack* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END