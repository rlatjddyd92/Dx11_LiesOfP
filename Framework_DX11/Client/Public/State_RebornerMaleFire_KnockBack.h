#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_RebornerMaleFire_KnockBack :
    public CState
{
private:
    typedef enum {
        AN_KNOCKBACK_L = 29, AN_KNOCKBACK_R = 30, AN_KNOCKBACK_B = 28
    }ANIMNUM;

public:
    CState_RebornerMaleFire_KnockBack(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_RebornerMaleFire_KnockBack() = default;

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
    static CState_RebornerMaleFire_KnockBack* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END