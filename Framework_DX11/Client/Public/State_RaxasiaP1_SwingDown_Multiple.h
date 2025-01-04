#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_RaxasiaP1_SwingDown_Multiple : public CState
{
private:
    typedef enum {
        AN_SWINGDOWN = 25, AN_SWINGDOWN_R = 26, AN_SWINGDOWN_L = 27
    }ANIMNUM;

public:
    CState_RaxasiaP1_SwingDown_Multiple(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_RaxasiaP1_SwingDown_Multiple() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _float              m_fCurrentTime = { 0.f };
    _float              m_fChaseDuration = { 1.5f };

    _Vec4               m_vRimLightColor = {};
    _bool               m_isRimLight = { false };

    _int                m_iCurAnimIndex = {};

    _bool               m_bSpeedController = { false };

    _bool               m_bSwing = { false };
    _bool               m_bStamp = { false };

    _bool               m_bSwingSound = { false };
    _bool               m_bStampSound = { false };

    _uint               m_iRouteTrack = {};

private:
    _bool               End_Check();
    void                Collider_Check(_double CurTrackPos);
    void                Effect_Check(_double CurTrackPos);
    void                Update_Rimlight(_float fTimeDelta, _double CurTrackPos);
    void                Control_Sound(_double CurTrackPos);

public:
    static CState_RaxasiaP1_SwingDown_Multiple* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END