#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_CarcassNormal_Walk : public CState
{
private:
    typedef enum {
        AN_WALK_F = 125, AN_WALK_B = 126, AN_WALK_L = 127, AN_WALK_R = 128,
    }ANIMNUM;

public:
    CState_CarcassNormal_Walk(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_CarcassNormal_Walk() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _float          m_fWalkSpeed = { 1.5f };
public:
    static CState_CarcassNormal_Walk* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END