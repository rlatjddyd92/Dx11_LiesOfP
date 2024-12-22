#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_CurruptedStrongArm_KnockBack :
    public CState
{
private:
    typedef enum {
        AN_KNOCKBACK_L = 24, AN_KNOCKBACK_R = 25, AN_KNOCKBACK_B = 21
    }ANIMNUM;

public:
    CState_CurruptedStrongArm_KnockBack(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_CurruptedStrongArm_KnockBack() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iCurAnim = {};

private:
    _bool               End_Check();

public:
    static CState_CurruptedStrongArm_KnockBack* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END