#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_RaxasiaP2_Running_Fury : public CState
{
private:
    typedef enum {
        AN_RUNNING = 39, AN_STING = 20
    }ANIMNUM;

public:
    CState_RaxasiaP2_Running_Fury(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_RaxasiaP2_Running_Fury() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _float              m_fDistance = {};

    _Vec4               m_vRimLightColor = {};
    _bool               m_isRimLight = { false };

    _bool               m_bRunningWise = { false }; //방향 구분용으로 사용. false면 왼, true면 오른
    _bool               m_bSwing = { false };
    _bool               m_bShieldAttack = { false };
    _bool               m_bStart = { false };

    _bool               m_bStartForSound = { false };
    _bool               m_bSwingSound = { false };
    _bool               m_bChargeForSound = { false };

    _bool               m_bSpeedController = { false };
    _bool               m_bLockOn = { false };

    _Vec3               m_vLockVec = {};

    _uint               m_iRouteTrack = {};

private:
    _bool               End_Check();
    void                Collider_Check(_double CurTrackPos);
    void                Effect_Check(_double CurTrackPos);
    void                Update_Rimlight(_float fTimeDelta, _double CurTrackPos);
    void                Control_Sound(_double CurTrackPos);

public:
    static CState_RaxasiaP2_Running_Fury* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END