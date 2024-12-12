#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_RaxasiaP2_Running_Linked : public CState
{
private:
    typedef enum {
        AN_LINKED_FIRST = 19, AN_LINKED_SECOND = 18, AN_JUMPLIGHTNING = 32
    }ANIMNUM;

public:
    CState_RaxasiaP2_Running_Linked(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_RaxasiaP2_Running_Linked() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _float              m_fCurrentTime = { 0.f };
    _float              m_fChaseDuration = { 1.5f };

    _bool               m_bSpeedController = { false };
    _bool               m_bSwing = { false };

    _bool               m_bSwingSound = { false };

    _uint               m_iRouteTrack = {};

private:
    _bool               End_Check();
    void                Collider_Check(_double CurTrackPos);
    void                Effect_Check(_double CurTrackPos);
    void                Control_Sound(_double CurTrackPos);

public:
    static CState_RaxasiaP2_Running_Linked* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END