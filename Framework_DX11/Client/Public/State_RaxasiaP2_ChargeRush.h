#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_RaxasiaP2_ChargeRush : public CState
{
private:
    typedef enum {
        AN_CHARGE = 28, AN_CHARGE_LOOP = 27, AN_SHIELDRUSH = 29
    }ANIMNUM;

public:
    CState_RaxasiaP2_ChargeRush(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_RaxasiaP2_ChargeRush() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };


    _Vec4               m_vRimLightColor = {};
    _bool               m_isRimLight = { false };

    _bool               m_bSpeedController = { false };

    _bool               m_bCharge = { false };
    _bool               m_bRush = { false };
    _bool               m_bSwing = { false };

    _bool               m_bSwingSound = { false };
    _bool               m_bChargeSound = { false };
    _bool               m_bRushSound = { false };

    _uint               m_iRouteTrack = {};

private:
    _bool               End_Check();
    void                Collider_Check(_double CurTrackPos);
    void                Effect_Check(_double CurTrackPos);
    void                Update_Rimlight(_float fTimeDelta, _double CurTrackPos);
    void                Control_Sound(_double CurTrackPos);

public:
    static CState_RaxasiaP2_ChargeRush* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END