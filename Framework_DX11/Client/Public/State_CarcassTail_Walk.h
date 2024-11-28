#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_CarcassTail_Walk : public CState
{
private:
    typedef enum {
        AN_WALK = 52, AN_TURN_LEFT = 49, AN_TURN_RIGHT = 50
    }ANIMNUM;
public:
    CState_CarcassTail_Walk(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_CarcassTail_Walk() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _float          m_fWalkSpeed = { 2.f };
public:
    static CState_CarcassTail_Walk* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END