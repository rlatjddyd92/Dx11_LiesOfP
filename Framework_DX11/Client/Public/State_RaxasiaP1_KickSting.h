#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_RaxasiaP1_KickSting : public CState
{
private:
    typedef enum {
        AN_KICKSTING = 23, AN_JUMPATTACK = 22
    }ANIMNUM;

public:
    CState_RaxasiaP1_KickSting(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_RaxasiaP1_KickSting() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _float              m_fCurrentTime = { 0.f };
    _float              m_fChaseDuration = { 1.5f };

    _bool               m_bSwing = { false };

    _bool               m_bChargeActive = { false };
    _bool               m_bStampBlast = { false };

    _bool               m_bSwingSound = { false };
    _bool               m_bStampSound = { false };

    _uint               m_iRouteTrack = {};

private:
    _bool               End_Check();
    void                Collider_Check(_double CurTrackPos);
    void                Effect_Check(_double CurTrackPos);
    void                Control_Sound(_double CurTrackPos);

public:
    static CState_RaxasiaP1_KickSting* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END