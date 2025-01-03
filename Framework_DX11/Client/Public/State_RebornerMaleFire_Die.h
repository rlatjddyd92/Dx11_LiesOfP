#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_RebornerMaleFire_Die :
    public CState
{
private:
    typedef enum {
        AN_DIE_F = 22, AN_DIE_B = 21
        , AN_DIE_F_DOWN = 1, AN_DIE_B_DOWN = 0
    }ANIMNUM;

public:
    CState_RebornerMaleFire_Die(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_RebornerMaleFire_Die() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iDirCnt = {};

    _bool               m_bExplosion = {false};

private:
    _bool               End_Check();
    void                Effect_Check(_double CurTrackPos);

public:
    static CState_RebornerMaleFire_Die* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END