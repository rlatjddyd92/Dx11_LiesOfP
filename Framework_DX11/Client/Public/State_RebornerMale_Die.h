#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_RebornerMale_Die :
    public CState
{
private:
    typedef enum {
        AN_DIE_F = 36, AN_DIE_B = 35
        , AN_DIE_F_DOWN = 17, AN_DIE_B_DOWN = 16
    }ANIMNUM;

public:
    CState_RebornerMale_Die(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_RebornerMale_Die() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iDirCnt = {};

private:
    _bool               End_Check();

public:
    static CState_RebornerMale_Die* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END