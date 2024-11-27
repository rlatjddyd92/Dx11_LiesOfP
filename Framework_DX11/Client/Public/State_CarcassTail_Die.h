#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_CarcassTail_Die :
    public CState
{
private:
    typedef enum {
        AN_DIE_F = 41, AN_DIE_B = 40
    }ANIMNUM;

public:
    CState_CarcassTail_Die(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_CarcassTail_Die() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

private:
    _bool               End_Check();

public:
    static CState_CarcassTail_Die* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END