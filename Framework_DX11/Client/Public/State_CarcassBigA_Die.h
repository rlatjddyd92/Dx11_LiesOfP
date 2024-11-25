#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_CarcassBigA_Die :
    public CState
{
private:
    typedef enum {
        AN_DIE_START_F = 14, AN_DIE_START_B = 13,
        AN_DIE_LOOP_F = 12, AN_DIE_LOOP_B = 11,
        AN_DIE_END_F = 10, AN_DIE_END_B = 9
    }ANIMNUM;
private:
    typedef enum {
        DIR_FRONT, DIR_BEHIND
    }DIR;

public:
    CState_CarcassBigA_Die(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_CarcassBigA_Die() = default;

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
    static CState_CarcassBigA_Die* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END