#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_SimonManusP1_Charge_SwingDown : public CState
{
private:
    typedef enum {
        AN_CHARGESWINGDOWN = 35
    }ANIMNUM;
public:
    CState_SimonManusP1_Charge_SwingDown(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_SimonManusP1_Charge_SwingDown() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _bool           m_bChargeSwing = { true };

private:
    _bool               End_Check();
    void                Collider_Check(_double CurTrackPos);
    void                Effect_Check(_double CurTrackPos);

public:
    static CState_SimonManusP1_Charge_SwingDown* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END