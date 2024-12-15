#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_RaxasiaP1_GroundSlash : public CState
{
private:
    typedef enum {
        AN_GROUNDSLASH = 24
    }ANIMNUM;

    typedef enum {
        NS_DISCHARGE, NS_SWINGDOWNMULTIPLE, NS_JUMPATTACK,
    }NEXTSTATE;

public:
    CState_RaxasiaP1_GroundSlash(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_RaxasiaP1_GroundSlash() = default;

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

    _bool               m_bSwingSound = { false };
    _bool               m_bResetList = { false };
    _bool               m_bInchent = { false };

    _bool               m_bAddMark = { false };

    _uint               m_iRouteTrack = {};

private:
    _bool               End_Check();
    void                Collider_Check(_double CurTrackPos);
    void                Effect_Check(_double CurTrackPos);
    void                Control_Sound(_double CurTrackPos);

public:
    static CState_RaxasiaP1_GroundSlash* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END