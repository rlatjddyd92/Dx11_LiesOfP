#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_CarcassNormal_KnockBack :
    public CState
{
private:
    typedef enum {
        AN_KNOCKBACK_L = 113, AN_KNOCKBACK_R = 117, AN_KNOCKBACK_B = 109
    }ANIMNUM;

public:
    CState_CarcassNormal_KnockBack(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_CarcassNormal_KnockBack() = default;

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
    static CState_CarcassNormal_KnockBack* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END