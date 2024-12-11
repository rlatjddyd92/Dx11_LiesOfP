#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_RaxasiaP1_Walk : public CState
{
private:
    typedef enum {
        AN_WALK_BACK = 45, AN_WALK_FRONT = 46, AN_WALK_LEFT = 47, AN_WALK_RIGHT = 48
    }ANIMNUM;

public:
    CState_RaxasiaP1_Walk(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_RaxasiaP1_Walk() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _float          m_fWalkSpeed = { 3.f };
    _float          m_fAttackDelay = {};

public:
    static CState_RaxasiaP1_Walk* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END